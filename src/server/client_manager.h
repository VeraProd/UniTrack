// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_CLIENT_MANAGER_H
#define SERVER_CLIENT_MANAGER_H

#include <memory>
#include <list>
#include <unordered_map>

#include <boost/asio.hpp>

#include <logger/logger.h>
#include <server/protocol_http.h>


namespace server {


typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr_t;


class worker;


class client_manager
{
public:
	client_manager(logger::logger &logger,
				   worker &w,
				   std::list<client_manager>::const_iterator it,
				   socket_ptr_t socket_ptr);
	
	// Non-copy/-move constructable/assignable.
	client_manager(const client_manager &other) = delete;
	client_manager(client_manager &&other) = delete;
	
	client_manager & operator=(const client_manager &other) = delete;
	client_manager & operator=(client_manager &&other) = delete;
private:
	// Closes the socket and removes manager from worker
	void finish_work();
	
	void headers_handler(const boost::system::error_code &err,
						 size_t bytes_transferred);
	
	// Data
	logger::logger &logger_;
	
	worker &worker_;
	std::list<client_manager>::const_iterator iterator_;
	
	socket_ptr_t socket_ptr_;
	
	
	// Request data
	boost::asio::streambuf headers_buf_;
	
	server::http::method method_;
	std::string uri_;
	std::unordered_map<std::string, std::string> headers_;
};	// class client_manager


};	// namespace server

#endif // SERVER_CLIENT_MANAGER_H
