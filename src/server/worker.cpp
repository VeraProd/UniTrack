// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/worker.h>
using namespace boost::asio;

#include <iostream>
#include <functional>


server::worker::worker(worker_id_t id,
					   boost::asio::io_service &io_service,
					   boost::asio::ip::tcp::acceptor &acceptor):
	id_(id),
	
	io_service_(io_service),
	acceptor_(acceptor),
	signal_set_(io_service_, SIGTERM, SIGINT, SIGQUIT),
	
	worker_thread_(std::bind(&worker::run, this))
{}


// protected
void
server::worker::run()
{
	using namespace std::placeholders;
	
	this->signal_set_.async_wait(std::bind(&worker::signal_handler, this, _1, _2));
	this->add_accept_handler();
	this->io_service_.run();
}


void
server::worker::stop()
{
	this->io_service_.stop();
}


void
server::worker::signal_handler(const boost::system::error_code &err,
							   int signal_number)
{
	// Stopping this worker
	this->stop();
	
	if (err) {
		std::cerr << "Error: Worker " << this->id() << " catched signal: "
				  << signal_number << ": " << err.message() << std::endl;
	} else {
		std::cerr << "Stopping worker " << this->id() << "..." << std::endl;
	}
}


void
server::worker::accept_handler(socket_ptr_t socket_ptr,
							   const boost::system::error_code &err)
{
	// Continue accepting...
	this->add_accept_handler();
	
	if (err) {
		std::cerr << "Error accepting socket: " << err.message() << std::endl;
	} else {
		this->add_client(socket_ptr);
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


void
server::worker::add_client(socket_ptr_t socket_ptr)
{
	this->client_managers_.emplace_back(this->io_service_, socket_ptr);
}
