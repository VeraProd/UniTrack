// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_SERVER_HTTP_H
#define SERVER_SERVER_HTTP_H

#include <thread>
#include <memory>
#include <vector>
#include <unordered_map>

#include <logger/logger.h>
#include <server/acceptor.h>
#include <server/worker.h>
#include <server/hosts_manager.h>
#include <server/types.h>


namespace server {


struct server_parameters
{
	server::port_set_t	ports					= {};
	unsigned int		workers					= 1;
};


class server_http
{
public:
	server_http(logger::logger &logger,
				const server_parameters &parameters);
	
	
	// Non-copy/-move constructable/assignable. Use ptrs.
	server_http(const server_http &other) = delete;
	server_http(server_http &&other) = delete;
	
	server_http & operator=(const server_http &other) = delete;
	server_http & operator=(server_http &&other) = delete;
	
	
	// Stops the server (waiting for server thread and workers threads)
	void stop() noexcept;
	
	
	inline bool joinable() const noexcept;	// Checks server's thread for joinable
	inline void join();						// Joins server's thread
	inline void detach();					// Detaches server's thread
protected:
	friend class server::acceptor;
	
	// Dispatches client pointed by socket_ptr to one of worker threads
	// Only acceptor calls this!
	void dispatch_client(server::socket_ptr_t socket_ptr) noexcept;
private:
	// Dispatches client to one of workers
	// WARNING: this method calls from one of workers' threads, NOT from server thread!
	// Only dispatch_client() calls this!
	void dispatch_client_worker_thread(server::socket_ptr_t socket_ptr) noexcept;
	
	
	void run() noexcept;
	
	
	// Data
	logger::logger &logger_;
	
	server_parameters parameters_;
	
	
	boost::asio::io_service acceptors_io_service_;	// Only for acceptors (server runs it in separate thread)!
	std::vector<std::unique_ptr<server::acceptor>> acceptor_ptrs_;
	boost::asio::io_service::work acceptor_empty_work_;	// If no acceptors required
	
	
	boost::asio::io_service workers_io_service_;	// Only for workers (each worker runs it in separate thread)!
	std::vector<std::unique_ptr<server::worker>> worker_ptrs_;
	std::unordered_map<std::thread::id, worker_id_t> workers_dispatch_table_;
	
	
	std::thread server_thread_;
};	// class server_http


};	// namespace server


#include <server/server_http.hpp>

#endif // SERVER_SERVER_HTTP_H
