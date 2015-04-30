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
	explicit headers_has_content_length(const std::string &value);
};


};	// namespace server

#endif // SERVER_HOST_EXCEPTIONS_H
