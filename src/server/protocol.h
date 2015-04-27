// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include <string>
#include <stdexcept>

#include <server/protocol_http.h>


namespace server {


struct start_data
{
	server::http::method method		= server::http::method::unknown;
	server::http::version version	= server::http::version::unknown;
	
	std::string uri;
};	// struct start_data


// Exceptions
class protocol_error: public std::logic_error
{
public:
	explicit protocol_error(const std::string &what_arg):
		std::logic_error(what_arg)
	{}
};


class incorrect_start_string: public protocol_error
{
public:
	explicit incorrect_start_string(const std::string &str):
		protocol_error("Incorrect start string: " + str)
	{}
};


class unimplemented_method: public protocol_error
{
public:
	explicit unimplemented_method(const std::string &method_name):
		protocol_error("Unimplemented method: " + method_name)
	{}
};


class incorrect_protocol: public protocol_error
{
public:
	explicit incorrect_protocol(const std::string &protocol_name):
		protocol_error("Incorrect protocol: " + protocol_name)
	{}
};


class unsupported_protocol_version: public protocol_error
{
public:
	explicit unsupported_protocol_version(const std::string &version):
		protocol_error("Unsupported protocol version: " + version)
	{}
};


// Parses the given string and checks all details except uri
// Returns all data from start string or throws one of exceptions abowe
start_data parse_start_string(const std::string &str);


};	// namespace server

#endif // SERVER_PROTOCOL_H
