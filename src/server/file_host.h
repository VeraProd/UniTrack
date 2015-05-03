// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_FILE_HOST_H
#define SERVER_FILE_HOST_H

#include <string>
#include <vector>
#include <regex>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <logger/logger.h>
#include <server/host.h>
#include <server/protocol.h>
#include <server/types.h>


namespace server {


struct file_host_only_parameters
{
	// May be useful, if several allow_regexes specified.
	enum class allow_match_mode
	{
		any,	// Uri matches any of allow_regexes (and doesn't match any of deny_regexes).
		all		// Uri matches all of allow_regexes (and doesn't match any of deny_regexes).
	};
	
	
	std::string root;
	
	std::vector<std::regex>
		deny_regexes =
			{
				std::regex(".*\\.\\./.*"),	// Don't allow "../" sequences!
				std::regex(".*/\\..*")		// Don't allow hidden directories and files
			},
		allow_regexes =
			{
				// Don't allow anything by default
			};
	
	allow_match_mode allow_match_mode;
};


struct file_host_parameters:
	public host_parameters,
	public file_host_only_parameters
{};


class file_host: public server::host
{
public:
	class cache: public server::host::cache
	{
	public:
		// Data
		boost::interprocess::file_mapping	file_mapping;
		boost::interprocess::mapped_region	mapped_region;
	};
	
	typedef std::shared_ptr<cache> cache_ptr_t;
	
	
	file_host(logger::logger &logger,
			  const file_host_parameters &parameters);
	
	
	// Prepares a correct response to the client. By default -- phony "404 Not Found".
	// Returns vector<buffer> ready to socket.async_send().
	// WARNING: result of this function does NOT contain the data, only references,
	// so rebember to save the data anywhere. uri, request_headers, strings_cache
	// and response_headers must be correct during all socket.async_send()!
	// strings_cache will contain some cached data.
	virtual
	std::pair<server::host::send_buffers_t, server::host::cache_ptr_t>
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
};	// class file_host


};	// namespace server


#endif // SERVER_FILE_HOST_H
