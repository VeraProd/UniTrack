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
private:
	// Closes the socket and removes manager from worker
	void finish_work() noexcept;
	
	void headers_handler(const boost::system::error_code &err,
						 size_t bytes_transferred);
	
	
	// Data
	logger::logger &logger_;
	
	worker &worker_;
	const_iterator_t iterator_;
	
	socket_ptr_t socket_ptr_;
	std::string client_ip_address_;
	
	// Request data
	boost::asio::streambuf headers_buf_;
	
	server::start_data start_data_;
	std::unordered_map<std::string, std::string> headers_;
};	// class client_manager


typedef client_manager::ptr_t client_manager_ptr_t;
typedef client_manager::list_t client_manager_list_t;
typedef client_manager::const_iterator_t client_manager_list_const_iterator_t;


};	// namespace server


#include <server/client_manager.hpp>

#endif // SERVER_CLIENT_MANAGER_H
