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

#include <base/buffer.h>

#include <logger/logger.h>

#include <server/protocol.h>
#include <server/types.h>
#include <server/host_parameters.h>


namespace server {


// Server knows about all existing hosts. If client needs host, that does exist,
// server dispatches request to the error_host.
class host:
	protected logger::enable_logger
{
public:
	host(logger::logger &logger,
		 const server::host_parameters &parameters);
	
	virtual ~host() = default;
	
	
	// Returns host's name (name does not include port information!).
	inline const std::string & name() const noexcept;
	
	
	// Returns true, if host can process requests on specified port, or false otherwise.
	bool port_allowed(server::port_t port) const noexcept;
	
	
	// Returns host name as string (random!).
	const std::string & server_name() const noexcept;
	
	
	std::pair<const std::string *, bool>
	server_name(const server::headers_t &response_headers,
				const server::headers_t &additional_headers) const;
	
	
	// Prepares a correct response to the client. By default -- phony "404 Not Found".
	// Returns pair<vector<buffer>, shared_ptr<cache>> ready to socket.async_send().
	// WARNING: first field of result does NOT contain data, only references. Second field
	// contains data need to be sent, so save the given shared_ptr anywhere during all sending!
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
	// WARNING: see notes to the response() method, remember to save anywhere status too
	// (standard statuses are already saved)! response_headers must NOT contain "Content-Length"!
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
	static void add_start_string(base::send_buffers_t &buffers,
								 server::http::version version,
								 const server::http::status &status);
	
	static void add_header(base::send_buffers_t &buffers,
						   const std::string &header,
						   const std::string &value);
	
	static void add_header(base::send_buffers_t &buffers,
						   const server::header_pair_t &header);
	
	static void add_headers(base::send_buffers_t &buffers,
							const server::headers_t &headers);
	
	static void finish_headers(base::send_buffers_t &buffers);
	
	// For body
	static void add_buffer(base::send_buffers_t &buffers,
						   const base::send_buffer_t &buffer);
	
	static void validate_headers(const headers_t &headers);
	
	
	// URI parsing
	static bool parse_uri(const std::string &uri, server::host_cache &cache);
protected:
	server::host_parameters host_parameters_;
private:
	static std::unique_ptr<host> error_host_;
	
	mutable std::minstd_rand0 server_name_generator_;
};	// class host


typedef std::shared_ptr<host> host_ptr_t;


};	// namespace server


#include <server/host.hpp>

#endif // SERVER_HOST_H
