// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_EXCEPTIONS_H
#define SERVER_HOST_EXCEPTIONS_H

#include <string>
#include <stdexcept>


namespace server {


class host_error: public std::logic_error
{
public:
	explicit host_error(const std::string &what_arg);
};


class headers_has_content_length: public host_error
{
public:
	explicit headers_has_content_length();
};


class duplicate_header: public host_error
{
public:
	explicit duplicate_header(const std::string &name);
};


class host_not_found: public host_error
{
public:
	explicit host_not_found(const std::string &name);
};


};	// namespace server

#endif // SERVER_HOST_EXCEPTIONS_H
