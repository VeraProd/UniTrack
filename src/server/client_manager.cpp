// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/client_manager.h>


server::client_manager::client_manager(boost::asio::ip::tcp::socket &&socket):
	socket_(std::move(socket))
{}
