// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_HTTP_H
#define SERVER_PROTOCOL_HTTP_H


namespace server {
namespace http {


enum class method
{
	GET,
	HEAD,
	POST,
	DELETE,
	
	unknown
};


enum class version
{
	_1_1,	// HTTP/1.1
	
	unknown
};


};	// namespace http
};	// namespace server

#endif // SERVER_PROTOCOL_HTTP_H
