// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/client_manager.h>

#include <iostream>
#include <functional>

#include <server/worker.h>
#include <server/protocol_exceptions.h>
#include <server/host_exceptions.h>
#include <server/client_manager_exceptions.h>


server::client_manager::client_manager(logger::logger &logger,
									   worker &w,
									   const_iterator_t iterator,
									   server::socket_ptr_t socket_ptr):
	logger_(logger),
	
	worker_(w),
	iterator_(iterator),
	
	running_operations_(0),
	
	socket_ptr_(socket_ptr)
{
	unique_lock_t lock(*this);
	
	// Getting client ip address
	try {
		auto endpoint = this->socket_ptr_->remote_endpoint();
		auto address = endpoint.address();
		this->client_ip_address_ = std::move(address.to_string());
	} catch (const boost::system::system_error &e) {
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): Unknown error with client: " << e.what() << '.';
		throw server::bad_client();
	}
	
	
	this->logger_.stream(logger::level::info)
		<< "Client manager (worker " << this->worker_.id()
		<< "): Client connected: " << this->client_ip_address() << '.';
	
	
	// Starting http-headers reading
	this->add_request_handler();
}


server::client_manager::~client_manager()
{
	// Closing the socket
	boost::system::error_code err;
	this->socket_ptr_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
	this->socket_ptr_->close();
}


// protected
// Mutex-like functions (for usage with std::unique_lock), but provides reference counting
// Tells current object does not destroy itself
void
server::client_manager::lock() noexcept
{
	++this->running_operations_;
}


// Removes manager from worker, if no works running
void
server::client_manager::unlock() noexcept
{
	--this->running_operations_;
	
	if (this->running_operations_ <= 0)
		this->worker_.erase_client(this->iterator_);
}


void
server::client_manager::log_error(const char *what, const server::http::status &status)
{
	this->logger_.stream(logger::level::error)
		<< "Client manager (worker " << this->worker_.id()
		<< "): " << this->client_ip_address() << ": " << what
		<< " => " << status.code() << '.';
}


void
server::client_manager::handle_error(const char *what,
									 const server::http::status &status,
									 bool exit,
									 bool send_phony,
									 server::headers_t &&headers)
{
	this->log_error(what, status);
	
	if (exit) this->keep_alive(false);
	if (send_phony) this->send_phony(status, std::move(headers));
}


void
server::client_manager::add_request_handler()
{
	this->lock();
	
	auto headers_buf_ptr = std::make_shared<boost::asio::streambuf>();
	
	using namespace std::placeholders;
	boost::asio::async_read_until(*this->socket_ptr_,
								  *headers_buf_ptr,
								  "\r\n\r\n",
								  std::bind(&client_manager::request_handler, this,
											headers_buf_ptr, _1, _2));
	
	this->logger_.stream(logger::level::info)
		<< "Client manager (worker " << this->worker_.id()
		<< "): " << this->client_ip_address()
		<< ": Waiting for headers...";
}


void
server::client_manager::send_response(server::host::response_data_t &&data)
{
	this->lock();
	
	using namespace std::placeholders;
	this->socket_ptr_->async_send(data.first,
								  std::bind(&client_manager::response_handler, this, data.second, _1, _2));
}


void
server::client_manager::send_phony(const server::http::status &status,
								   server::headers_t &&headers)
{
	// Getting data for the phony...
	auto data = std::move(server::host::error_host(this->logger_).phony_response(
		this->connection_params_.version,
		status,
		std::move(headers)));
	
	// ...and sending it
	this->send_response(std::move(data));
}


// private
void
server::client_manager::parse_headers(server::client_manager::streambuf_ptr_t headers_buf_ptr)
{
	std::istream headers_stream(headers_buf_ptr.get());
	std::string str;
	
	
	// Processing start string
	std::getline(headers_stream, str);
	
	{
		auto start_data = std::move(server::parse_start_string(str));
		
		this->connection_params_.method = start_data.method;
		this->connection_params_.version = start_data.version;
		this->connection_params_.uri = std::move(start_data.uri);
	}
	
	
	// Processing headers
	while (std::getline(headers_stream, str) && !str.empty()) {
		try {
			this->connection_params_.headers.insert(std::move(server::parse_header_string(str)));
		} catch (const server::empty_header_string &)
		{}	// It's normal for the last string
	}
	
	
	// Setting up keep-alive
	try {
		static const std::regex keep_alive_regex(".*keep-alive.*", std::regex::optimize);
		
		this->keep_alive(false);
		
		if (regex_match(this->connection_params_.headers.at(server::http::header_connection),
						keep_alive_regex))
			this->keep_alive(true);
	} catch (...) {}
}


void
server::client_manager::request_handler(server::client_manager::streambuf_ptr_t headers_buf_ptr,
										const boost::system::error_code &err,
										size_t bytes_transferred)
{
	unique_lock_t lock(*this);
	this->unlock();
	
	if (err) {
		if (err == boost::asio::error::misc_errors::eof) {
			this->logger_.stream(logger::level::info)
				<< "Client manager (worker " << this->worker_.id()
				<< "): Client disconnected: " << this->client_ip_address() << '.';
		} else {
			this->logger_.stream(logger::level::error)
				<< "Client manager (worker " << this->worker_.id()
				<< "): " << this->client_ip_address()
				<< ": " << err.value() << " " << err.message() << '.';
		}
		return;
	}
	
	
	try {
		this->parse_headers(headers_buf_ptr);
		
		
		{
			auto stream = this->logger_.stream(logger::level::info);
			stream
				<< "Client manager (worker " << this->worker_.id()
				<< "): " << this->client_ip_address()
				<< ": HTTP/" << server::http::version_to_str(this->connection_params_.version)
				<< ", " << server::http::method_to_str(this->connection_params_.method)
				<< ", Requested URI: \"" << this->connection_params_.uri << "\". Headers:";
			
			for (const auto &p: this->connection_params_.headers)
				stream << " [" << p.first << ": " << p.second << ']';
			stream << '.';
		}
		
		
		if (this->keep_alive())
			this->add_request_handler();
		
		
		// ok
		this->send_phony(server::http::status::ok);
	} catch (const server::unimplemented_method &e) {
		// not_implemented
		this->handle_error(
			e, server::http::status::not_implemented,
			true, true);
	} catch (const server::unsupported_protocol_version &e) {
		// http_version_not_supported
		this->handle_error(
			e, server::http::status::http_version_not_supported,
			true, true);
	} catch (const server::incorrect_header_string &e) {
		// unrecoverable_error
		this->handle_error(
			e, server::http::status::unrecoverable_error,
			true, true);
	} catch (const server::protocol_error &e) {
		// Catches incorrect_protocol and incorrect_start_string too
		
		// unrecoverable_error
		this->handle_error(
			e, server::http::status::unrecoverable_error,
			true, true);
	} catch (const server::host_error &e) {
		// internal_server_error
		this->handle_error(
			e, server::http::status::internal_server_error,
			true, true);
	} catch (...) {
		// internal_server_error
		this->handle_error(
			"Unknown error", server::http::status::internal_server_error,
			true, true);
	}
}


void
server::client_manager::response_handler(server::host::cache_ptr_t cache_ptr,
										 const boost::system::error_code &err,
										 size_t bytes_transferred)
{
	unique_lock_t lock(*this);
	this->unlock();
	
	if (err) {
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << this->client_ip_address()
			<< ": Error sending response: " << err.message() << '.';
	} else {
		this->logger_.stream(logger::level::info)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << this->client_ip_address()
			<< ": Response sent.";
	}
}
