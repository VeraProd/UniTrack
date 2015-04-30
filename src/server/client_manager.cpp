// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/client_manager.h>

#include <iostream>
#include <functional>

#include <server/worker.h>
#include <server/protocol_exceptions.h>


server::client_manager::client_manager(logger::logger &logger,
									   worker &w,
									   const_iterator_t iterator,
									   socket_ptr_t socket_ptr):
	logger_(logger),
	
	worker_(w),
	iterator_(iterator),
	
	socket_ptr_(socket_ptr)
{
	// Getting client ip address
	try {
		auto endpoint = this->socket_ptr_->remote_endpoint();
		auto address = endpoint.address();
		this->client_ip_address_ = std::move(address.to_string());
	} catch (const boost::system::system_error &e) {
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): Unknown error with client: " << e.what() << '.';
		this->finish_work();
		return;
	}
	
	this->logger_.stream(logger::level::info)
		<< "Client manager (worker " << this->worker_.id()
		<< "): Client connected: " << this->client_ip_address() << '.';
	
	
	// Starting http-headers reading
	{
		using namespace std::placeholders;
		boost::asio::async_read_until(*this->socket_ptr_,
									  this->headers_buf_,
									  "\r\n\r\n",
									  std::bind(&client_manager::request_handler, this, _1, _2));
	}
}


server::client_manager::~client_manager()
{
	// Closing the socket
	boost::system::error_code err;
	this->socket_ptr_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
	this->socket_ptr_->close();
}


// protected
void
server::client_manager::send_response(server::host::send_buffers_t &&buffers)
{
	using namespace std::placeholders;
	
	this->socket_ptr_->async_send(buffers,
								  std::bind(&client_manager::response_handler, this, _1, _2));
}


// private
// Closes the socket and removes manager from worker
void
server::client_manager::finish_work() noexcept
{
	this->worker_.erase_client(this->iterator_);
}


void
server::client_manager::request_handler(const boost::system::error_code &err,
										size_t bytes_transferred)
{
	try {
		std::istream headers_stream(&this->headers_buf_);
		std::string str;
		
		
		// Processing start string
		std::getline(headers_stream, str);
		this->start_data_ = std::move(server::parse_start_string(str));
		
		// Processing headers
		while (std::getline(headers_stream, str) && !str.empty()) {
			try {
				this->headers_.insert(std::move(server::parse_header_string(str)));
			} catch (const server::empty_header_string &)
			{}	// It's normal for the last string
		}
		
		
		auto stream = this->logger_.stream(logger::level::info);
		stream
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << this->client_ip_address()
			<< ": HTTP/" << server::http::version_to_str(this->start_data_.version)
			<< ", " << server::http::method_to_str(this->start_data_.method)
			<< ", Requested URI: \"" << this->start_data_.uri << "\". Headers:";
		
		for (const auto &p: this->headers_)
			stream << " [" << p.first << ": " << p.second << ']';
		stream << '.';
		
		
		// ok
		auto buffers = std::move(server::host::error_host(this->logger_).phony_page(
			this->start_data_.version,
			server::http::status::ok,
			this->host_cache_));
		this->send_response(std::move(buffers));
	} catch (const server::unimplemented_method &e) {
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << this->client_ip_address() << ": " << e.what()
			<< " => " << server::http::status::method_not_allowed.code() << '.';
		
		// method_not_allowed
		
		this->finish_work();
		return;
	} catch (const server::unsupported_protocol_version &e) {
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << this->client_ip_address() << ": " << e.what()
			<< " => " << server::http::status::http_version_not_supported.code() << '.';
		
		// http_version_not_supported
		
		this->finish_work();
		return;
	} catch (const server::incorrect_header_string &e) {
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << this->client_ip_address() << ": " << e.what()
			<< " => " << server::http::status::unrecoverable_error.code() << '.';
		
		// unrecoverable_error
		
		this->finish_work();
		return;
	} catch (const server::protocol_error &e) {
		// Catches incorrect_protocol and incorrect_start_string too
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << this->client_ip_address() << ": " << e.what()
			<< " => " << server::http::status::unrecoverable_error.code() << '.';
		
		// unrecoverable_error
		
		this->finish_work();
		return;
	} catch (...) {
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << this->client_ip_address() << ": Unknown error"
			<< " => " << server::http::status::internal_server_error.code() << '.';
		
		// internal_server_error
		
		this->finish_work();
		return;
	}
}


void
server::client_manager::response_handler(const boost::system::error_code &err,
										 size_t bytes_transferred)
{
	this->host_cache_.clear();
	
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
	
	this->finish_work();
	return;
}
