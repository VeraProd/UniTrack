// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/client_manager.h>

#include <iostream>


server::client_manager::client_manager(boost::asio::io_service &io_service,
									   socket_ptr_t socket_ptr):
	io_service_(io_service),
	socket_ptr_(socket_ptr)
{
	std::cerr << "Connected!!!" << std::endl;
}
