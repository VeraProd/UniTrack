// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_H
#define SERVER_HOST_H

#include <string>
#include <vector>
#include <list>
#include <unordered_set>
#include <memory>
#include <random>

#include <boost/asio/buffer.hpp>

#include <logger/logger.h>
#include <server/protocol.h>
#include <server/types.h>


namespace server {


// Host parameters. Need for every host.
// This is default parameters for error_host.
struct host_parameters
{
	std::string					name;
	port_set_t					ports;
	
	std::vector<std::string>	server_names =
		{
			"UniTrack/0.0.1",
			"Tired of reading the headers? Look for the UniTrack project on GitHub!",
			"ZX_Spectrum/1997 (Sinclair_BASIC)",
			"c64/1986 (**** COMMODORE 64 BASIC V2 **** 64K RAM SYSTEM ****)",
			"thttpd/1.02 (Minix 2.0.2 i186)",
			"pear/6.2 (iOS 5.0.1)",
			"segasrv/1.0 (SEGA MEGA DRIVE HTTP SERVER ROM 1.0)",
			"Tea with milk (Full cup, with sugar)"
		};
};


// Server knows about all existing hosts. If client needs host, that does exist,
// server dispatches request to the error_host.
class host
{
public:
	class cache
	{
	public:
		virtual ~cache() {}
		
		
		// Data
		std::string				uri;
		
		server::headers_t		request_headers,
								response_headers,
								additional_headers;
		
		std::list<std::string>	strings;
	};	// class cache
	
	typedef std::shared_ptr<cache> cache_ptr_t;
	
	typedef boost::asio::const_buffer send_buffer_t;
	typedef std::vector<send_buffer_t> send_buffers_t;
	
	typedef std::pair<send_buffers_t, cache_ptr_t> response_data_t;
	
	
	host(logger::logger &logger,
		 const host_parameters &parameters);
	
	virtual ~host();
	
	
	// Returns host's name (name does not include port information!).
	inline const std::string & name() const noexcept;
	
	
	// Returns true, if host can process requests on specified port, or false otherwise.
	bool port_allowed(port_t port) const noexcept;
	
	
	// Returns host name as string (random!).
	const std::string & server_name() const noexcept;
	
	
	std::pair<const std::string *, bool>
	server_name(const server::headers_t &response_headers,
				const server::headers_t &additional_headers) const;
	
	
	// Prepares a correct response to the client. By default -- phony "404 Not Found".
	// Returns vector<buffer> ready to socket.async_send().
	// WARNING: result of this function does NOT contain the data, only references,
	// so rebember to save the data anywhere. uri, request_headers, strings_cache
	// and response_headers must be correct during all socket.async_send()!
	// strings_cache will contain some cached data.
	virtual
	response_data_t
	response(
		std::string &&uri,						// Requested URI
		server::http::method method,			// GET, POST, ...
		server::http::version version,			// _1_1, ...
		headers_t &&request_headers,			// All headers given by client. Must NOT be
												// included in response, need only for host.
		headers_t &&response_headers = {});		// Headers must be added to response.
	
	
	// Prepares a phony response to the client.
	// Returns vector<buffer> ready to socket.async_send().
	// WARNING: see notes to the response() method, remember to save anywhere status too!
	// response_headers must NOT contain "Content-Length"!
	response_data_t
	phony_response(server::http::version version,
				   const server::http::status &status,
				   headers_t &&response_headers = {},
				   headers_t &&additional_headers = {});
	
	
	// Returns reference to error host object, creating it, if does not exist.
	// If it didn't exist, it will be binded to logger of object's, whoose method was called.
	host & error_host() const;
	
	// Returns reference to error host object, creating it, if does not exist.
	// If it didn't exist, it will be binded to logger.
	static host & error_host(logger::logger &logger);
	
	// Creates error_host if it does not exist. You may call it once from server, if you want.
	static void create_error_host(logger::logger &logger);
	
	
	// Response forming helpers
	// For headers
	static void add_start_string(send_buffers_t &buffers,
								 server::http::version version,
								 const server::http::status &status);
	
	static void add_header(send_buffers_t &buffers,
						   const std::string &header,
						   const std::string &value);
	
	static void add_header(send_buffers_t &buffers,
						   const server::header_pair_t &header);
	
	static void add_headers(send_buffers_t &buffers,
							const server::headers_t &headers);
	
	static void finish_headers(send_buffers_t &buffers);
	
	// For body
	static void add_buffer(send_buffers_t &buffers,
						   const send_buffer_t &buffer);
	
	static void validate_headers(const headers_t &headers);
	
	
	inline logger::logger & logger() const noexcept;
protected:
	host_parameters host_parameters_;
private:
	static std::unique_ptr<host> error_host_;
	
	logger::logger &logger_;
	
	mutable std::minstd_rand0 server_name_generator_;
};	// class host


typedef std::shared_ptr<host> host_ptr_t;


};	// namespace server


#include <server/host.hpp>

#endif // SERVER_HOST_H
