// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_SERVER_HTTP_H
#define SERVER_SERVER_HTTP_H

#include <vector>

#include <boost/asio.hpp>

#include <server/worker.h>


namespace server {


struct server_parameters
{
	unsigned short port		= 8080;
	unsigned int workers	= 1;
};


class server_http
{
public:
	server_http(const server_parameters &parameters);
	
	void run();
	void stop();
protected:
	void start_worker();
private:
	server_parameters parameters_;
	
	boost::asio::io_service io_service_;
	boost::asio::ip::tcp::endpoint endpoint_;
	
	std::vector<worker> workers_;
};	// class server_http


};	// namespace server

#endif // SERVER_SERVER_HTTP_H
