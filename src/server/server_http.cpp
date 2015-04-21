// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/server_http.h>
using namespace boost::asio;

#include <iostream>
#include <functional>
#include <thread>


server::server_http::server_http(server_parameters parameters):
	parameters_(parameters),
	
	io_service_(),
	endpoint_(ip::tcp::v4(), parameters_.port)
{
	worker_id_t worker_id = 0;
	auto workers_count = this->parameters_.workers;
	this->workers_.reserve(workers_count);
	
	while (workers_count--) {
		this->workers_.emplace_back(worker_id++,
									this->io_service_,
									this->endpoint_);
	}
}


void
server::server_http::run()
{
	this->io_service_.run();
}


void
server::server_http::stop()
{
	this->io_service_.stop();
}
