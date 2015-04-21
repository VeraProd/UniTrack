// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/server_http.h>
using namespace boost::asio;

#include <iostream>
#include <functional>
#include <thread>


server::server_http::server_http(unsigned short port_num):
	io_service_(),
	endpoint_(ip::tcp::v4(), port_num),
	acceptor_(io_service_)
{}


void
server::server_http::run()
{
	this->add_accept_handler();
	this->io_service_.run();
}


void
server::server_http::stop()
{
	this->io_service_.stop();
}


// protected
void
server::server_http::accept_handler(std::shared_ptr<boost::asio::ip::tcp::socket> socket,
									const boost::system::error_code &err)
{
	this->add_accept_handler();
	if (err) {
		std::cerr << "Error accepting socket: " << err.message() << std::endl;
	} else {
		// Starting client_manager in new thread
		std::thread([socket]() {
			client_manager manager(std::move(*socket));
		}).detach();
	}
}


void
server::server_http::add_accept_handler()
{
	using namespace std::placeholders;
	
	auto socket = std::make_shared<ip::tcp::socket>(this->io_service_);
	this->acceptor_.async_accept(*socket,
								 std::bind(&server_http::accept_handler,
										   this, socket, _1));
}
