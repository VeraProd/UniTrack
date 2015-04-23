// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/client_manager.h>


server::client_manager::client_manager(logger::logger &logger,
									   boost::asio::io_service &io_service,
									   socket_ptr_t socket_ptr):
	logger_(logger),
	io_service_(io_service),
	socket_ptr_(socket_ptr)
{
	this->logger_.stream(logger::level::info)
		<< "Connected!!!";
}
