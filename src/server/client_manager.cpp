// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/client_manager.h>

#include <iostream>


server::client_manager::client_manager(boost::asio::io_service &io_service,
									   boost::asio::ip::tcp::socket &&socket):
	io_service_(io_service),
	socket_(std::move(socket))
{
	std::cerr << "Connected!!!" << std::endl;
}
