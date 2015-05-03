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
#include <server/types.h>


namespace server {


class worker;


class client_manager//:
	// public std::enable_shared_from_this<client_manager>
{
public:
	typedef std::shared_ptr<client_manager> ptr_t;
	typedef std::list<ptr_t> list_t;
	typedef list_t::const_iterator const_iterator_t;
	
	
	client_manager(logger::logger &logger,
				   worker &w,
				   const_iterator_t iterator,
				   server::socket_ptr_t socket_ptr);
	~client_manager();
	
	
	// Non-copy/-move constructable/assignable.
	client_manager(const client_manager &other) = delete;
	client_manager(client_manager &&other) = delete;
	
	client_manager & operator=(const client_manager &other) = delete;
	client_manager & operator=(client_manager &&other) = delete;
	
	
	inline const std::string &client_ip_address() const;
	inline bool keep_alive() const;
protected:
	inline void keep_alive(bool status);
	
	
	// Mutex-like functions (for usage with std::unique_lock), but provides reference counting
	friend class std::unique_lock<client_manager>;
	typedef std::unique_lock<client_manager> unique_lock_t;
	
	// Tells current object does not destroy itself
	void lock() noexcept;
	
	// Removes manager from worker, if no works running
	void unlock() noexcept;
	
	
	void log_error(const char *what, const server::http::status &status);
	
	template<class Exception>
	inline void log_error(const Exception &e, const server::http::status &status);
	
	
	void handle_error(const char *what,
					  const server::http::status &status,
					  bool exit,
					  bool send_phony,
					  server::headers_t &&headers = {});
	
	template<class Exception>
	inline void handle_error(const Exception &e,
							 const server::http::status &status,
							 bool exit,
							 bool send_phony,
							 server::headers_t &&headers = {});
	
	
	void add_request_handler();
	
	void send_response(server::host::response_data_t &&data);
	void send_phony(const server::http::status &status,
					server::headers_t &&headers = {});
private:
	typedef std::shared_ptr<boost::asio::streambuf> streambuf_ptr_t;
	
	
	void parse_headers(streambuf_ptr_t streambuf_ptr);
	
	
	// Handlers
	void request_handler(streambuf_ptr_t streambuf_ptr,
						 const boost::system::error_code &err,
						 size_t bytes_transferred);
	
	void response_handler(server::host::cache_ptr_t cache_ptr,
						  const boost::system::error_code &err,
						  size_t bytes_transferred);
	
	
	// Data
	logger::logger &logger_;
	
	worker &worker_;
	const_iterator_t iterator_;
	
	unsigned int running_operations_;
	
	
	server::socket_ptr_t socket_ptr_;
	std::string client_ip_address_;
	
	
	// Session data
	struct {
		server::http::method	method;
		server::http::version	version;
		std::string				uri;
		
		server::headers_t		headers;
		
		bool					keep_alive;
	} connection_params_;
};	// class client_manager


typedef client_manager::ptr_t client_manager_ptr_t;
typedef client_manager::list_t client_manager_list_t;
typedef client_manager::const_iterator_t client_manager_list_const_iterator_t;


};	// namespace server


#include <server/client_manager.hpp>

#endif // SERVER_CLIENT_MANAGER_H
