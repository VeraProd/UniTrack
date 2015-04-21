// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_CLIENT_MANAGER_H
#define SERVER_CLIENT_MANAGER_H

#include <boost/asio.hpp>


namespace server {


class client_manager
{
public:
	client_manager(boost::asio::io_service &io_service,
				   boost::asio::ip::tcp::socket &&socket);
private:
	boost::asio::io_service &io_service_;
	boost::asio::ip::tcp::socket socket_;
};	// class client_manager


};	// namespace server

#endif // SERVER_CLIENT_MANAGER_H
