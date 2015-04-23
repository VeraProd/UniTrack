// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include <thread>
#include <list>

#include <boost/asio.hpp>

#include <server/client_manager.h>


namespace server {


typedef unsigned int worker_id_t;


class worker
{
public:
	worker(worker_id_t id,
		   boost::asio::io_service &io_service,
		   boost::asio::ip::tcp::acceptor &acceptor);
	
	inline worker_id_t id() const
	{ return this->id_; }
private:
	void run();		// Must be called in worker_thread_ thread
	void stop();	// Stops worker
	
	// Handlers
	void signal_handler(const boost::system::error_code &err,
						int signal_number);
	
	void accept_handler(socket_ptr_t socket_ptr,
						const boost::system::error_code &err);
	
	void add_accept_handler();
	
	void add_client(socket_ptr_t socket_ptr);
	
	
	worker_id_t id_;
	
	boost::asio::io_service &io_service_;
	boost::asio::ip::tcp::acceptor &acceptor_;
	boost::asio::signal_set signal_set_;
	
	std::list<client_manager> client_managers_;
	
	std::thread worker_thread_;
};	// class worker


};	// namespace server

#endif // SERVER_WORKER_H
