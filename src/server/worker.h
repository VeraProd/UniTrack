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
	
	
	// Non-copy/-move constructable/assignable. Use ptrs.
	worker(const worker &other) = delete;
	worker(worker &&other) = delete;
	
	worker & operator=(const worker &other) = delete;
	worker & operator=(worker &&other) = delete;
	
	
	// Returns worker id
	inline worker_id_t id() const noexcept;
	
	// Returns worker thread id (need for server's dispatcher)
	inline std::thread::id thread_id() const noexcept;
	
	
	// Adds new client to the worker
	// Returns true, if added successfully
	bool add_client(socket_ptr_t socket_ptr) noexcept;
	
	
	inline bool joinable() const noexcept;	// Checks worker's thread for joinable
	inline void join();						// Joins worker's thread
	inline void detach();					// Detaches worker's thread
	
	
	// Erases client by iterator. Client manager uses this.
	void erase_client(client_manager_list_const_iterator_t iterator) noexcept;
private:
	// Must be called in worker_thread_ thread
	// NOTE: Constructor calls this automatically. Do NOT call it manually!
	void run() noexcept;
	
	// Stops the worker (canceling all incoming clients)
	// NOTE: Do NOT call this manually! Worker's run() does it.
	void stop() noexcept;
	
	
	// Data
	logger::logger &logger_;
	
	worker_parameters parameters_;
	
	boost::asio::io_service &io_service_;
	boost::asio::io_service::work work_;
	
	client_manager_list_t client_managers_;	// Clients, worker are working with
	
	std::thread worker_thread_;
};	// class worker


};	// namespace server


#include <server/worker.hpp>

#endif // SERVER_WORKER_H
