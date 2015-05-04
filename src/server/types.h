// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_TYPES_H
#define SERVER_TYPES_H

#include <string>
#include <memory>
#include <vector>
#include <list>
#include <unordered_set>
#include <unordered_map>

#include <boost/asio.hpp>


namespace server {


// Protocols
typedef std::pair<std::string, std::string> header_pair_t;
typedef std::unordered_map<std::string, std::string> headers_t;


// Sockets
typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr_t;


// Ports
typedef unsigned short				port_t;
typedef std::unordered_set<port_t>	port_set_t;


// Workers
typedef unsigned int worker_id_t;


// Hosts
class host_cache
{
public:
	typedef std::shared_ptr<host_cache> ptr_t;
	
	
	virtual ~host_cache() {}
	
	
	// Data
	std::string				uri;
	
	server::headers_t		request_headers,
							response_headers,
							additional_headers;
	
	std::list<std::string>	strings;
};	// class cache


typedef boost::asio::const_buffer send_buffer_t;
typedef std::vector<send_buffer_t> send_buffers_t;

typedef std::pair<send_buffers_t, host_cache::ptr_t> response_data_t;


// File host
template<class HostType>
class file_host_cache:
	public server::host_cache,
	public HostType::cache
{
public:
	typedef std::shared_ptr<file_host_cache> ptr_t;
};	// class file_host_cache


};	// namespace server


#endif // SERVER_TYPES_H
