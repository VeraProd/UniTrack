// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_CLIENT_MANAGER_H
#define SERVER_CLIENT_MANAGER_H

#include <memory>

#include <boost/asio.hpp>

#include <logger/logger.h>


namespace server {


typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr_t;


class client_manager
{
public:
	client_manager(logger::logger &logger,
				   boost::asio::io_service &io_service,
				   socket_ptr_t socket_ptr);
private:
	// Data
	logger::logger &logger_;
	
	boost::asio::io_service &io_service_;
	socket_ptr_t socket_ptr_;
};	// class client_manager


};	// namespace server

#endif // SERVER_CLIENT_MANAGER_H
