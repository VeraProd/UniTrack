// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/server_http.h>
using namespace boost::asio;

#include <iostream>
#include <functional>
#include <thread>


server::server_http::server_http(const server_parameters &parameters):
	parameters_(parameters),
	
	io_service_(),
	endpoint_(ip::tcp::v4(), parameters_.port),
	acceptor_(io_service_, endpoint_),
	signal_set_(io_service_, SIGTERM, SIGINT, SIGQUIT)
{
	worker_id_t worker_id = 0;
	auto workers_count = this->parameters_.workers;
	this->workers_.reserve(workers_count);
	
	while (workers_count--) {
		this->workers_.emplace_back(new worker(worker_id++,
											   this->io_service_,
											   this->acceptor_));
	}
}


void
server::server_http::run()
{
	using namespace std::placeholders;
	
	this->signal_set_.async_wait(std::bind(&server_http::signal_handler, this, _1, _2));
	this->io_service_.run();
}


// private
void
server::server_http::stop()
{
	this->io_service_.stop();
}


void
server::server_http::signal_handler(const boost::system::error_code &err,
									int signal_number)
{
	// Stopping the server
	this->stop();
	
	if (err) {
		std::cerr << "Error: Server catched signal: " << signal_number << ": "
				  << err.message() << std::endl;
	} else {
		std::clog << "Stopping server..." << std::endl;
	}
}
