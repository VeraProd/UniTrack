// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/client_manager.h>

#include <iostream>
#include <functional>
#include <regex>

#include <server/worker.h>
#include <server/protocol_exceptions.h>
#include <server/host_exceptions.h>


server::client_manager::client_manager(logger::logger &logger,
									   worker &w,
									   const_iterator_t iterator,
									   server::socket_ptr_t socket_ptr,
									   server::host_manager &host_manager):
	logger_(logger),
	
	host_manager_(host_manager),
	
	worker_(w),
	iterator_(iterator),
	
	running_operations_(0),
	
	socket_ptr_(socket_ptr),
	client_ip_address_(std::move(socket_ptr_->remote_endpoint().address().to_string())),
	server_port_(socket_ptr_->local_endpoint().port()),
	keep_alive_(false),
	
	sending_(false)
{
	unique_lock_t lock(*this);
	
	
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
server::client_manager::handle_error(server::client_manager::request_data_ptr_t request_data_ptr,
									 const char *what,
									 const server::http::status &status,
									 bool exit,
									 bool send_phony,
									 server::headers_t &&headers)
{
	this->log_error(what, status);
	
	if (exit) this->keep_alive(false);
	if (send_phony) this->send_phony(request_data_ptr, status, std::move(headers));
}


void
server::client_manager::add_request_handler()
{
	this->lock();
	
	auto request_data_ptr = std::make_shared<server::client_manager::request_data>();
	
	using namespace std::placeholders;
	boost::asio::async_read_until(*this->socket_ptr_,
								  request_data_ptr->headers_buf,
								  "\r\n\r\n",
								  std::bind(&client_manager::request_handler, this,
											request_data_ptr, _1, _2));
	
	this->logger_.stream(logger::level::info)
		<< "Client manager (worker " << this->worker_.id()
		<< "): " << this->client_ip_address()
		<< ": Waiting for headers...";
}


void
server::client_manager::send_response(server::response_data_t &&data)
{
	this->data_to_send_.push(std::move(data));
	this->add_response_handler();
}


void
server::client_manager::send_phony(server::client_manager::request_data_ptr_t request_data_ptr,
								   const server::http::status &status,
								   server::headers_t &&headers)
{
	// Getting data for the phony...
	auto data = std::move(server::host::error_host(this->logger_).phony_response(
		request_data_ptr->version,
		status,
		std::move(headers)));
	
	// ...and sending it
	this->send_response(std::move(data));
}


// private
// Helpers
void
server::client_manager::parse_headers(server::client_manager::request_data_ptr_t request_data_ptr)
{
	std::istream headers_stream(&request_data_ptr->headers_buf);
	std::string str;
	
	
	// Processing start string
	std::getline(headers_stream, str);
	
	{
		auto start_data = std::move(server::parse_start_string(str));
		
		request_data_ptr->method = start_data.method;
		request_data_ptr->version = start_data.version;
		request_data_ptr->uri = std::move(start_data.uri);
	}
	
	
	// Processing headers
	while (std::getline(headers_stream, str) && !str.empty()) {
		try {
			request_data_ptr->headers.insert(std::move(server::parse_header_string(str)));
		} catch (const server::empty_header_string &)
		{}	// It's normal for the last string
	}
	
	
	// Setting up keep-alive
	try {
		static const std::regex keep_alive_regex(".*keep-alive.*", std::regex::optimize);
		
		this->keep_alive(false);
		
		if (regex_match(request_data_ptr->headers.at(server::http::header_connection),
						keep_alive_regex))
			this->keep_alive(true);
	} catch (...) {}
}


void
server::client_manager::process_request(request_data_ptr_t request_data_ptr)
{
	static const std::regex host_regex("([^:]+)"				// Host name [1]
									   "(:([[:digit:]]+))?",	// Port number, if specified [3]
									   std::regex::optimize);
	
	auto it = request_data_ptr->headers.find(server::http::header_host);
	if (it == request_data_ptr->headers.end())
		throw server::header_required(server::http::header_host);
	
	
	// Host checking
	const std::string &host_str = it->second;
	
	std::smatch match_result;
	if (!std::regex_match(host_str, match_result, host_regex) || match_result.size() != 4)
		throw server::incorrect_host_header(host_str);
	
	
	// Port checking
	auto port = this->server_port();
	{
		std::string requested_port;
		if (match_result.length(3) == 0) requested_port = server::http::default_port_str;	// 80 port
		else requested_port = match_result[3];
		
		if (std::to_string(port) != requested_port) {
			this->logger_.stream(logger::level::sec_warning)
				<< "Client manager (worker " << this->worker_.id()
				<< "): " << this->client_ip_address()
				<< ": Incorrect port in Host header: \"" << host_str << "\".";
			throw server::incorrect_port(requested_port);
		}
	}
	
	
	auto host_ptr = this->host_manager_.host(match_result[1], port);
	
	this->send_response(std::move(host_ptr->response(std::move(request_data_ptr->uri),
													 request_data_ptr->method,
													 request_data_ptr->version,
													 std::move(request_data_ptr->headers))));
}


void
server::client_manager::request_handler(server::client_manager::request_data_ptr_t request_data_ptr,
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
		this->parse_headers(request_data_ptr);
		
		
		{
			auto stream = this->logger_.stream(logger::level::info);
			stream
				<< "Client manager (worker " << this->worker_.id()
				<< "): " << this->client_ip_address()
				<< ": HTTP/" << server::http::version_to_str(request_data_ptr->version)
				<< ", " << server::http::method_to_str(request_data_ptr->method)
				<< ", Requested URI: \"" << request_data_ptr->uri << "\". Headers:";
			
			for (const auto &p: request_data_ptr->headers)
				stream << " [" << p.first << ": " << p.second << ']';
			stream << '.';
		}
		
		
		if (this->keep_alive())
			this->add_request_handler();
		
		
		// ok
		this->process_request(request_data_ptr);
	} catch (const server::unimplemented_method &e) {					// Protocol errors
		this->handle_error(request_data_ptr, e,
						   server::http::status::not_implemented,
						   true, true);
	} catch (const server::unsupported_protocol_version &e) {
		this->handle_error(request_data_ptr, e,
						   server::http::status::http_version_not_supported,
						   true, true);
	} catch (const server::protocol_error &e) {
		// Also catches incorrect_protocol, incorrect_start_string, incorrect_header_string,
		// header_required, incorrect_host_header, incorrect_port
		
		this->handle_error(request_data_ptr, e,
						   server::http::status::bad_request,
						   true, true);
	} catch (const server::host_not_found &e) {							// Host errors
		this->handle_error(request_data_ptr, e,
						   server::http::status::not_found,
						   true, true);
	} catch (const server::host_error &e) {
		// Also catches headers_has_content_length, duplicate_header
		
		this->handle_error(request_data_ptr, e,
						   server::http::status::internal_server_error,
						   true, true);
	} catch (...) {														// Other errors
		this->handle_error(request_data_ptr, "Unknown error",
						   server::http::status::internal_server_error,
						   true, true);
	}
}


void
server::client_manager::add_response_handler()
{
	if (this->data_to_send_.empty()) return;
	
	if (!this->sending_) {
		this->sending_ = true;
		this->lock();
		
		auto data = std::move(this->data_to_send_.front());
		this->data_to_send_.pop();
		
		using namespace std::placeholders;
		this->socket_ptr_->async_send(data.first,
									  std::bind(&client_manager::response_handler, this,
												data.second, _1, _2));
	}
}


void
server::client_manager::response_handler(server::host_cache::ptr_t cache_ptr,
										 const boost::system::error_code &err,
										 size_t bytes_transferred)
{
	unique_lock_t lock(*this);
	this->unlock();
	this->sending_ = false;
	
	this->add_response_handler();
	
	
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
