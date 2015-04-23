// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include <thread>
#include <list>

#include <boost/asio.hpp>
#include <boost/lockfree/spsc_queue.hpp>

#include <server/client_manager.h>
#include <logger/logger.h>


namespace server {


typedef unsigned int worker_id_t;


class worker
{
public:
	worker(logger::logger &logger,
		   worker_id_t id,
		   boost::asio::io_service &io_service);
	
	
	// Returns worker id
	inline worker_id_t id() const
	{ return this->id_; }
	
	
	// Adds new client to the worker
	// Returns true, if added successfully
	bool add_client(socket_ptr_t socket_ptr);
	
	
	void join();	// Joins worker's thread
	void detach();	// Detaches worker's thread
private:
	void run();		// Must be called in worker_thread_ thread
	void stop();	// Stops the worker
	
	
	// Data
	logger::logger &logger_;
	
	worker_id_t id_;
	
	boost::asio::io_service &io_service_;

	boost::lockfree::spsc_queue<socket_ptr_t> incoming_clients_;
	std::list<client_manager> client_managers_;
	
	std::thread worker_thread_;
};	// class worker


};	// namespace server

#endif // SERVER_WORKER_H
