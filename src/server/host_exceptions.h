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


class denied_uri: public host_error
{
public:
	explicit denied_uri(const std::string &uri);
};


class method_not_allowed: public host_error
{
public:
	explicit method_not_allowed(const std::string &method_name);
};


class path_forbidden: public host_error
{
public:
	explicit path_forbidden(const std::string &path);
};


class path_not_found: public host_error
{
public:
	explicit path_not_found(const std::string &path);
};


};	// namespace server


#endif // SERVER_HOST_EXCEPTIONS_H
