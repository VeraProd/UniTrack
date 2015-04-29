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
									  std::bind(&client_manager::headers_handler, this, _1, _2));
	}
}


server::client_manager::~client_manager()
{
	// Closing the socket
	boost::system::error_code err;
	this->socket_ptr_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
	this->socket_ptr_->close();
}


// private
// Closes the socket and removes manager from worker
void
server::client_manager::finish_work() noexcept
{
	this->worker_.erase_client(this->iterator_);
}


void
server::client_manager::headers_handler(const boost::system::error_code &err,
										size_t bytes_transferred)
{
	std::istream headers_stream(&this->headers_buf_);
	std::string str;
	
	// Processing start string
	try {
		std::getline(headers_stream, str);
		
		this->logger_.stream(logger::level::info)
			<< "Client manager (worker " << this->worker_.id()
			<< "): Got headers.";
		
		this->start_data_ = std::move(server::parse_start_string(str));
		
		this->logger_.stream(logger::level::info)
			<< "Client manager (worker " << this->worker_.id()
			<< "): Headers parsed.";
	} catch (const server::unimplemented_method &e) {
		// method_not_allowed
		this->finish_work();
		return;
	} catch (const server::unsupported_protocol_version &e) {
		// http_version_not_supported
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << e.what() << '.';
		this->finish_work();
		return;
	} catch (const server::incorrect_protocol &e) {
		// unrecoverable_error
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << e.what() << '.';
		this->finish_work();
		return;
	} catch (const server::incorrect_start_string &e) {
		// unrecoverable_error
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << e.what() << '.';
		this->finish_work();
		return;
	} catch (const server::protocol_error &e) {
		// unrecoverable_error
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): " << e.what() << '.';
		this->finish_work();
		return;
	} catch (...) {
		this->logger_.stream(logger::level::error)
			<< "Client manager (worker " << this->worker_.id()
			<< "): Unknown error.";
		
		// internal_server_error
		
		this->finish_work();
		return;
	}
	
	// ok
	
	this->logger_.stream(logger::level::info)
		<< "Client manager (worker " << this->worker_.id()
		<< "): HTTP/" << server::http::version_to_str(this->start_data_.version)
		<< ", " << server::http::method_to_str(this->start_data_.method)
		<< ", Requested URI: \"" << this->start_data_.uri << "\".";
}
