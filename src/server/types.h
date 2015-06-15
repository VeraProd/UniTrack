// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_TYPES_H
#define SERVER_TYPES_H

#include <string>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <iterator>

#include <base/cache.h>
#include <base/buffer.h>

#include <boost/asio/ip/tcp.hpp>
#include <boost/filesystem/path.hpp>


namespace server {


// Workers
typedef unsigned int worker_id_t;



// Sockets
typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr_t;


// Ports
typedef unsigned short				port_t;
typedef std::unordered_set<port_t>	port_set_t;



// Protocols
typedef std::pair<std::string, std::string>				header_pair_t;
typedef std::unordered_map<std::string, std::string>	headers_t;


// URI
typedef std::unordered_map<std::string, std::string>	uri_arguments_map_t;
typedef std::unordered_set<std::string>					uri_arguments_set_t;



// Hosts
class host_cache
{
public:
	// Types
	typedef					host_cache *	raw_ptr_t;
	typedef	std::unique_ptr<host_cache>		unique_ptr_t;
	typedef	std::shared_ptr<host_cache>		shared_ptr_t;
	
	
	// Data
	boost::filesystem::path		path;
	uri_arguments_map_t			args_map;
	uri_arguments_set_t			args_set;
	
	
	server::headers_t			request_headers,
								response_headers,
								additional_headers;
	
	
	base::strings_cache_t		strings;
};	// class host_cache


// Response
typedef std::pair<base::send_buffers_t, host_cache::shared_ptr_t> response_data_t;



// File host
template<class HostType>
class file_host_cache:
	public server::host_cache,
	public HostType::cache
{
public:
	// Types
	typedef					file_host_cache<HostType> *		raw_ptr_t;
	typedef	std::unique_ptr<file_host_cache<HostType>>		unique_ptr_t;
	typedef	std::shared_ptr<file_host_cache<HostType>>		shared_ptr_t;
	
	
	// Construct from server::host_cache
	file_host_cache(const server::host_cache &other):
		server::host_cache(other)
	{}
	
	file_host_cache(server::host_cache &&other):
		server::host_cache(std::move(other))
	{}
	
	
	// Construct from HostType::cache
	file_host_cache(const typename HostType::cache &other):
		HostType::cache(other)
	{}
	
	file_host_cache(typename HostType::cache &&other):
		HostType::cache(std::move(other))
	{}
	
	
	file_host_cache() = default;
	file_host_cache(const file_host_cache &other) = default;
	file_host_cache(file_host_cache &&other) = default;
	
	file_host_cache & operator=(const file_host_cache &other) = default;
	file_host_cache & operator=(file_host_cache &&other) = default;
	
	virtual ~file_host_cache() = default;
};	// class file_host_cache


};	// namespace server


#endif // SERVER_TYPES_H
