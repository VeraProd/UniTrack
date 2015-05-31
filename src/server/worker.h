// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include <thread>
#include <list>

#include <boost/asio.hpp>
#include <boost/lockfree/spsc_queue.hpp>

#include <logger/logger.h>
#include <server/client_manager.h>
#include <server/host_manager.h>
#include <server/types.h>


namespace server {


struct worker_parameters
{
	worker_id_t		id;
};


class worker:
	protected logger::enable_logger
{
public:
	worker(logger::logger &logger,
		   const worker_parameters &parameters,
		   boost::asio::io_service &io_service,
		   server::host_manager &host_manager);
	
	
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
	bool add_client(server::socket_ptr_t socket_ptr) noexcept;
	
	
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
	worker_parameters parameters_;
	
	server::host_manager &host_manager_;
	
	boost::asio::io_service &io_service_;
	boost::asio::io_service::work work_;
	
	server::client_manager_list_t client_managers_;	// Clients, worker are working with
	
	std::thread worker_thread_;
};	// class worker


};	// namespace server


#include <server/worker.hpp>

#endif // SERVER_WORKER_H
