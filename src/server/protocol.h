// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include <string>
#include <unordered_map>

#include <server/protocol_http.h>
#include <server/types.h>


namespace server {


// Removes trailing '\r' and '\n' symbols modifying the string
void truncate_string(std::string &str) noexcept;


struct start_data
{
	server::http::method method		= server::http::method::unknown;
	server::http::version version	= server::http::version::unknown;
	
	std::string uri;
};	// struct start_data


// Parses the given string and checks all details except uri
// Returns all data from start string or throws one of exceptions
// described in protocol_exceptions.h
start_data parse_start_string(const std::string &str);


header_pair_t parse_header_string(const std::string &str);


};	// namespace server

#endif // SERVER_PROTOCOL_H
