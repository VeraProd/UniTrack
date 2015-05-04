// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_CLIENT_MANAGER_H
#define SERVER_CLIENT_MANAGER_H

#include <string>
#include <memory>
#include <list>
#include <unordered_map>
#include <mutex>

#include <boost/asio.hpp>

#include <logger/logger.h>
#include <server/protocol.h>
#include <server/host.h>
#include <server/hosts_manager.h>
#include <server/types.h>


namespace server {


class worker;


class client_manager
{
public:
	typedef std::shared_ptr<client_manager> ptr_t;
	typedef std::list<ptr_t> list_t;
	typedef list_t::const_iterator const_iterator_t;
	
	
	client_manager(logger::logger &logger,
				   worker &w,
				   const_iterator_t iterator,
				   server::socket_ptr_t socket_ptr,
				   server::hosts_manager &hosts_manager);
	~client_manager();
	
	
	// Non-copy/-move constructable/assignable.
	client_manager(const client_manager &other) = delete;
	client_manager(client_manager &&other) = delete;
	
	client_manager & operator=(const client_manager &other) = delete;
	client_manager & operator=(client_manager &&other) = delete;
	
	
	inline const std::string &client_ip_address() const noexcept;
	inline port_t server_port() const noexcept;
	
	inline bool keep_alive() const noexcept;
protected:
	inline void keep_alive(bool status) noexcept;
	
	
	// Mutex-like functions (for usage with std::unique_lock), but provides reference counting
	friend class std::unique_lock<client_manager>;
	typedef std::unique_lock<client_manager> unique_lock_t;
	
	// Tells current object does not destroy itself
	void lock() noexcept;
	
	// Removes manager from worker, if no works running
	void unlock() noexcept;
	
	
	// Request data
	struct request_data
	{
		boost::asio::streambuf	headers_buf;
		
		server::http::method	method;
		server::http::version	version;
		std::string				uri;
		
		server::headers_t		headers;
	};	// struct request_data
	
	typedef std::shared_ptr<request_data> request_data_ptr_t;
	
	
	void log_error(const char *what, const server::http::status &status);
	
	
	void handle_error(request_data_ptr_t request_data_ptr,
					  const char *what,
					  const server::http::status &status,
					  bool exit,
					  bool send_phony,
					  server::headers_t &&headers = {});
	
	template<class Exception>
	inline void handle_error(request_data_ptr_t request_data_ptr,
							 const Exception &e,
							 const server::http::status &status,
							 bool exit,
							 bool send_phony,
							 server::headers_t &&headers = {});
	
	
	void add_request_handler();
	
	void send_response(server::response_data_t &&data);
	void send_phony(request_data_ptr_t request_data_ptr,
					const server::http::status &status,
					server::headers_t &&headers = {});
private:
	// Helpers
	void parse_headers(request_data_ptr_t request_data_ptr);
	
	void process_request(request_data_ptr_t request_data_ptr);
	
	
	// Handlers
	void request_handler(request_data_ptr_t request_data_ptr,
						 const boost::system::error_code &err,
						 size_t bytes_transferred);
	
	void response_handler(server::host_cache::ptr_t cache_ptr,
						  const boost::system::error_code &err,
						  size_t bytes_transferred);
	
	
	// Data
	logger::logger &logger_;
	
	server::hosts_manager &hosts_manager_;
	
	worker &worker_;
	const_iterator_t iterator_;
	
	unsigned int running_operations_;
	
	
	// Connection data
	server::socket_ptr_t	socket_ptr_;
	std::string				client_ip_address_;
	port_t					server_port_;
	bool					keep_alive_;
};	// class client_manager


typedef client_manager::ptr_t client_manager_ptr_t;
typedef client_manager::list_t client_manager_list_t;
typedef client_manager::const_iterator_t client_manager_list_const_iterator_t;


};	// namespace server


#include <server/client_manager.hpp>

#endif // SERVER_CLIENT_MANAGER_H
