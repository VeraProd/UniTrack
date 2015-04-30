// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_CLIENT_MANAGER_H
#define SERVER_CLIENT_MANAGER_H

#include <string>
#include <memory>
#include <list>
#include <unordered_map>

#include <boost/asio.hpp>

#include <logger/logger.h>
#include <server/protocol.h>
#include <server/host.h>


namespace server {


typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr_t;

typedef unsigned long client_id_t;


class worker;


class client_manager:
	public std::enable_shared_from_this<client_manager>
{
public:
	typedef std::shared_ptr<client_manager> ptr_t;
	typedef std::list<ptr_t> list_t;
	typedef list_t::const_iterator const_iterator_t;
	
	
	client_manager(logger::logger &logger,
				   worker &w,
				   const_iterator_t iterator,
				   socket_ptr_t socket_ptr);
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
	
	
	void log_error(const char *what, const server::http::status &status);
	
	template<class Exception>
	inline void log_error(const Exception &e, const server::http::status &status);
	
	
	void handle_error(const char *what,
					  const server::http::status &status,
					  bool exit,
					  bool send_phony,
					  std::unordered_map<std::string, std::string> &&headers = {});
	
	template<class Exception>
	inline void handle_error(const Exception &e,
							 const server::http::status &status,
							 bool exit,
							 bool send_phony,
							 std::unordered_map<std::string, std::string> &&headers = {});
	
	void add_request_handler();
	
	void send_response(server::host::send_buffers_t &&buffers);
	void send_phony(const server::http::status &status,
					std::unordered_map<std::string, std::string> &&headers = {});
	
	void parse_headers();
private:
	// Closes the socket and removes manager from worker
	void finish_work() noexcept;
	
	void request_handler(const boost::system::error_code &err,
						 size_t bytes_transferred);
	
	void response_handler(const boost::system::error_code &err,
						  size_t bytes_transferred);
	
	
	// Data
	logger::logger &logger_;
	
	worker &worker_;
	const_iterator_t iterator_;
	
	socket_ptr_t socket_ptr_;
	std::string client_ip_address_;
	
	// Session data
	server::start_data start_data_;
	struct {
		server::http::method	method;
		server::http::version	version;
		std::string				uri;
		
		std::unordered_map<std::string, std::string> headers;
		
		bool					keep_alive;
	} connection_params_;
	
	// Cached data
	struct {
		boost::asio::streambuf headers_buf;
		host::cache_t host;
		
		std::unordered_map<std::string, std::string> headers;
	} cache_;
};	// class client_manager


typedef client_manager::ptr_t client_manager_ptr_t;
typedef client_manager::list_t client_manager_list_t;
typedef client_manager::const_iterator_t client_manager_list_const_iterator_t;


};	// namespace server


#include <server/client_manager.hpp>

#endif // SERVER_CLIENT_MANAGER_H
