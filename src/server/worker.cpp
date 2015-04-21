// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/worker.h>

#include <functional>


server::worker::worker(worker_id_t id,
					   boost::asio::io_service &io_service,
					   boost::asio::ip::tcp::endpoint &endpoint):
	id_(id),
	
	io_service_(io_service),
	acceptor_(io_service_, endpoint),
	
	worker_thread_(std::bind(&worker::run, this))
{}


// protected
void
server::worker::run()
{
	this->io_service_.run();
}


void
server::worker::stop()
{
	this->io_service_.stop();
}


void
server::worker::accept_handler(std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr,
							   const boost::system::error_code &err)
{
	// Continue accepting...
	this->add_accept_handler();
	
	if (err) {
		std::cerr << "Error accepting socket: " << err.message() << std::endl;
	} else {
		// Adding new client_manager
		this->client_managers_.emplace_back(this->io_service_, std::move(*socket_ptr));
	}
}


void
server::worker::add_accept_handler()
{
	using namespace std::placeholders;
	
	auto socket_ptr = std::make_shared<ip::tcp::socket>(this->io_service_);
	this->acceptor_.async_accept(*socket_ptr,
								 std::bind(&worker::accept_handler,
										   this, socket_ptr, _1));
}
