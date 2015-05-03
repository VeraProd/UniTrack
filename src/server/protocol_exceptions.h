// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_EXCEPTIONS_H
#define SERVER_PROTOCOL_EXCEPTIONS_H

#include <string>
#include <stdexcept>


namespace server {


class protocol_error: public std::logic_error
{
public:
	explicit protocol_error(const std::string &what_arg);
};


// Start string
class incorrect_start_string: public protocol_error
{
public:
	explicit incorrect_start_string(const std::string &str);
};


class unimplemented_method: public protocol_error
{
public:
	explicit unimplemented_method(const std::string &method_name);
};


class incorrect_protocol: public protocol_error
{
public:
	explicit incorrect_protocol(const std::string &protocol_name);
};


class unsupported_protocol_version: public protocol_error
{
public:
	explicit unsupported_protocol_version(const std::string &version);
};


// Headers
class incorrect_header_string: public protocol_error
{
public:
	explicit incorrect_header_string(const std::string &str);
};


class empty_header_string: public protocol_error
{
public:
	explicit empty_header_string();
};


class header_required: public protocol_error
{
public:
	explicit header_required(const std::string &header_name);
};


class incorrect_host_header: public protocol_error
{
public:
	explicit incorrect_host_header(const std::string &host_str);
};


class incorrect_port: public protocol_error
{
public:
	explicit incorrect_port(const std::string &port_str);
};


};	// namespace server

#endif // SERVER_PROTOCOL_EXCEPTIONS_H
