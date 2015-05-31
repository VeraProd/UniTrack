// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/acceptor.h>

#include <functional>

#include <server/server_http.h>


server::acceptor::acceptor(logger::logger &logger,
						   server::server_http &server,
						   const acceptor_parameters &parameters,
						   boost::asio::io_service &acceptors_io_service,
						   boost::asio::io_service &workers_io_service):
	logger::enable_logger(logger),
	
	server_(server),
	parameters_(parameters),
	
	acceptors_io_service_(acceptors_io_service),
	workers_io_service_(workers_io_service),
	
	endpoint_(boost::asio::ip::tcp::v4(), parameters_.port),
	acceptor_(acceptors_io_service_, endpoint_)
{
	this->add_accept_handler();
	
	this->logger().stream(logger::level::info)
		<< "Acceptor: Accepting on port: " << this->parameters_.port
		<< ": started.";
}


server::acceptor::~acceptor()
{
	boost::system::error_code err;
	this->acceptor_.close(err);
}


// Handles the accept event
void
server::acceptor::accept_handler(server::socket_ptr_t socket_ptr,
								 const boost::system::error_code &err) noexcept
{
	if (err) {
		this->logger().stream(logger::level::error)
			<< "Acceptor: Accepting on port: " << this->parameters_.port
			<< ": " << err.message() << '.';
	} else {
		this->add_accept_handler();	// Continue accepting
		
		this->logger().stream(logger::level::info)
			<< "Acceptor: New connection on port: " << this->parameters_.port
			<< ": Accepted.";
	}
	
	this->server_.dispatch_client(socket_ptr);
}


// Add accept_handler to the io_service event loop
void
server::acceptor::add_accept_handler() noexcept
{
	using namespace std::placeholders;
	
	auto socket_ptr = std::make_shared<boost::asio::ip::tcp::socket>(this->workers_io_service_);
	this->acceptor_.async_accept(*socket_ptr,
								 std::bind(&acceptor::accept_handler,
										   this, socket_ptr, _1));
}
