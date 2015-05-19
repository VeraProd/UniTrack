// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_FILE_HOST_H
#define SERVER_FILE_HOST_H

#include <logger/logger.h>
#include <server/host.h>
#include <server/protocol.h>
#include <server/types.h>

#include <server/file_host_parameters.h>
#include <server/file_host_files_only.h>


namespace server {


// Requirements to the class HostType:
// 	- may have class inside:
// 		class cache (even it is empty), that have a default constructor
// 		
// 		Then you can leave CacheType template parameter as is. If not (for example, HostType
// 		is pointer to function or lambda), try to set CacheType to server::host_cache. But then
// 		HostType object will be unable to use own cache.
// 	- must have non-static method:
// 		
// 		std::pair<server::send_buffers_t, server::send_buffers_t>
// 		operator()(const std::string &path,				// File name the handler should process
// 				   CacheType::raw_ptr_t cache_ptr);		// Cache that the handler can use
// 		
// 		Return value: first if headers buffers, second is content buffers.
// 		Headers buffers must contain Content-Length, if need.
// 		This method can throws: server::path_forbidden or server::path_not_found.
// 		In this case status will be 403 Forbidden and 404 Not Found.
// 		Other exceptions will be processed with status 500 Internal Server Error.
// 		
// 		This method can use cache pointed by cache_ptr and must return buffers
// 		ready to socket.async_send(). All data is in that cache.
// 
// See also:
// - server/types.h for server::send_buffers_t and server::file_host_cache<>::ptr_t

// Requirements to class CacheType:
// 	- must be inheritor of server::host_cache
// 	- must have typedef inside: ptr_t


// NOTE: If you don't know what is these, and what should you do, simple files-only host example:
// 		server::file_host_parameters params;
// 		// Set parameters with: params.smth = smth_val;
// 		server::file_host<server::files_only> host(logger, params);


template<class HostType,
		 class CacheType = server::file_host_cache<HostType>>
class file_host: public server::host
{
public:
	typedef CacheType cache_t;
	typedef typename cache_t::ptr_t cache_ptr_t;
	
	
	file_host(logger::logger &logger,
			  const file_host_parameters &parameters,
			  HostType &&file_handler = std::move(HostType()));
	
	file_host(logger::logger &logger,
			  const file_host_parameters &parameters,
			  const HostType &file_handler);
	
	
	// Non-copy/-move constructable/assignable. Use ptrs.
	template<class HostType1, class CacheType1>
	file_host(const file_host<HostType1, CacheType1> &other) = delete;
	
	template<class HostType1, class CacheType1>
	file_host(file_host<HostType1, CacheType1> &&other) = delete;
	
	template<class HostType1, class CacheType1>
	file_host<HostType, CacheType> &
	operator=(const file_host<HostType1, CacheType1> &other) = delete;
	
	template<class HostType1, class CacheType1>
	file_host<HostType, CacheType> &
	operator=(file_host<HostType1, CacheType1> &&other) = delete;
	
	
	// Prepares a correct response to the client.
	// Returns pair<vector<buffer>, shared_ptr<cache>> ready to socket.async_send().
	// WARNING: first field of result does NOT contain data, only references. Second field
	// contains data need to be sent, so save the given shared_ptr anywhere during all sending!
	virtual
	std::pair<server::send_buffers_t, server::host_cache::ptr_t>
	response(std::string &&uri,
			 server::http::method method,
			 server::http::version version,
			 server::headers_t &&request_headers,
			 server::headers_t &&response_headers = {}) override;
protected:
	bool validate_uri(const std::string &uri) const noexcept;
	bool validate_method(server::http::method method) const noexcept;
	
	
	// Data
	file_host_only_parameters file_host_parameters_;
	
	HostType file_handler_;
};	// class file_host


};	// namespace server


#include <server/file_host.hpp>

#endif // SERVER_FILE_HOST_H
