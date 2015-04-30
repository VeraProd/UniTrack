// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_H
#define SERVER_HOST_H

#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <memory>
#include <random>

#include <boost/asio/buffer.hpp>

#include <logger/logger.h>
#include <server/protocol.h>


namespace server {


// Host parameters. Need for every host.
// This is default parameters for error_host.
struct host_parameters
{
	std::string					name;
	unsigned int				port;
	
	std::vector<std::string>	server_names = {
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
	typedef std::list<std::string> cache_t;
	typedef std::vector<boost::asio::const_buffer> send_buffers_t;
	
	
	host(logger::logger &logger,
		 const host_parameters &parameters);
	
	
	// Returns host name as string (random!)
	const std::string & server_name() const noexcept;
	
	
	// Returns vector<buffer> ready to socket.async_send()
	// WARNING: result of this function does NOT contain the data, only references,
	// so rebember to save the data anywhere. status and headers must be correct
	// during all socket.async_send()! strings_cache contains some cached data.
	// Caller must store it as long as it need too.
	send_buffers_t
	phony_page(server::http::version version,
			   const server::http::status &status,
			   cache_t &strings_cache,
			   const std::unordered_map<std::string, std::string> &headers = {});
	
	
	// Returns reference to error host object, creating it, if does not exist.
	// If it didn't exist, it will be binded to logger of object's, whoose method was called.
	host & error_host() const;
	
	// Returns reference to error host object, creating it, if does not exist.
	// If it didn't exist, it will be binded to logger.
	static host & error_host(logger::logger &logger);
	
	// Creates error_host if it does not exist. You may call it once from server, if you want.
	static void create_error_host(logger::logger &logger);
private:
	static std::unique_ptr<host> error_host_;
	
	logger::logger &logger_;
	host_parameters parameters_;
	
	mutable std::minstd_rand0 server_name_generator_;
};	// class host


};	// namespace server


#endif // SERVER_HOST_H
