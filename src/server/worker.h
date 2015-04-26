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


struct worker_parameters
{
	worker_id_t		id;
	unsigned int	max_incoming_clients	= 128;
	unsigned int	max_clients				= 1000;
};


class worker
{
public:
	worker(logger::logger &logger,
		   const worker_parameters &parameters,
		   boost::asio::io_service &io_service);
	
	
	// Returns worker id
	inline worker_id_t id() const
	{ return this->parameters_.id; }
	
	
	// Adds new client to the worker
	// Returns true, if added successfully
	bool add_client(socket_ptr_t socket_ptr);
	
	// Stops the worker (canceling all incoming clients)
	// NOTE: Must be called before server thread stops the io_service!
	void stop();
	
	inline bool joinable() const;	// Checks worker's thread for joinable
	inline void join();				// Joins worker's thread
	inline void detach();			// Detaches worker's thread
private:
	// Must be called in worker_thread_ thread
	// NOTE: Constructor calls this automatically. Do NOT call it manually!
	void run();
	
	
	// Data
	logger::logger &logger_;
	
	worker_parameters parameters_;
	
	boost::asio::io_service &io_service_;
	boost::asio::io_service::work work_;

	boost::lockfree::spsc_queue<socket_ptr_t> incoming_clients_;
	std::list<client_manager> client_managers_;	// Clients, worker are working with
	
	std::thread worker_thread_;
};	// class worker


};	// namespace server


#include <server/worker.hpp>

#endif // SERVER_WORKER_H
