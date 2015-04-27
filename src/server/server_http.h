// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_SERVER_HTTP_H
#define SERVER_SERVER_HTTP_H

#include <thread>
#include <memory>
#include <vector>
#include <unordered_map>

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
private:
	void run() noexcept;
	
	// Handlers
	// Handles the accept event
	void accept_handler(socket_ptr_t socket_ptr,
						const boost::system::error_code &err) noexcept;
	
	// Add accept_handler to the io_service event loop
	void add_accept_handler() noexcept;
	
	
	// Dispatches client to one of workers
	// WARNING: this method calls from one of workers' threads, NOT from server thread!
	// Only accept_handler() call this.
	void dispatch_client(socket_ptr_t socket_ptr) noexcept;
	
	
	// Data
	logger::logger &logger_;
	
	server_parameters parameters_;
	
	boost::asio::io_service server_io_service_,		// Only for server's async_accept!
							workers_io_service_;	// For all workers
	boost::asio::ip::tcp::endpoint endpoint_;
	boost::asio::ip::tcp::acceptor acceptor_;
	
	std::vector<std::unique_ptr<worker>> worker_ptrs_;
	std::unordered_map<std::thread::id, worker_id_t> workers_dispatch_table_;
	
	std::thread server_thread_;
};	// class server_http


};	// namespace server


#include <server/server_http.hpp>

#endif // SERVER_SERVER_HTTP_H
