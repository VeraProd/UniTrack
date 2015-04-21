// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_SERVER_HTTP_H
#define SERVER_SERVER_HTTP_H

#include <memory>

#include <boost/asio.hpp>

#include <server/client_manager.h>


namespace server {


class server_http
{
public:
	server_http(unsigned short port_num);
	
	void run();
	void stop();
protected:
	void accept_handler(std::shared_ptr<boost::asio::ip::tcp::socket> socket,
						const boost::system::error_code &err);
	void add_accept_handler();
private:
	boost::asio::io_service io_service_;
	boost::asio::ip::tcp::endpoint endpoint_;
	boost::asio::ip::tcp::acceptor acceptor_;
};	// class server_http


};	// namespace server

#endif // SERVER_SERVER_HTTP_H
