// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_SERVER_HTTP_H
#define SERVER_SERVER_HTTP_H

#include <thread>
#include <memory>
#include <vector>

#include <server/worker.h>
#include <logger/logger.h>


namespace server {


struct server_parameters
{
	unsigned short	port						= 8080;
	unsigned int	workers						= 1;
	
	// Workers parameters
	unsigned int	worker_max_incoming_clients	= 128;
	unsigned int	worker_max_clients			= 1000;
};


class server_http
{
public:
	server_http(logger::logger &logger,
				const server_parameters &parameters);
	
	void stop();
	
	inline bool joinable() const;	// Checks server's thread for joinable
	inline void join();				// Joins server's thread
	inline void detach();			// Detaches server's thread
private:
	void run();
	
	// Handlers
	void accept_handler(socket_ptr_t socket_ptr,
						const boost::system::error_code &err);
	
	void add_accept_handler();
	
	
	// Data
	logger::logger &logger_;
	
	server_parameters parameters_;
	
	boost::asio::io_service io_service_;
	boost::asio::ip::tcp::endpoint endpoint_;
	boost::asio::ip::tcp::acceptor acceptor_;
	
	std::vector<std::unique_ptr<worker>> workers_;
	worker_id_t current_worker_id_;
	
	std::thread server_thread_;
};	// class server_http


};	// namespace server


#include <server/server_http.hpp>

#endif // SERVER_SERVER_HTTP_H
