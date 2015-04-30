// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_PROTOCOL_HTTP_H
#define SERVER_PROTOCOL_HTTP_H

#include <string>


namespace server {
namespace http {


extern const std::string
	space_str,
	newline_str,
	HTTP_str,
	slash_str,
	header_separator_str,
	
	header_content_length,
	header_server,
	header_allow;


enum class method
{
	GET,
	HEAD,
	POST,
	DELETE,
	
	unknown
};


const std::string & method_to_str(method method_) noexcept;


enum class version
{
	_1_1,	// HTTP/1.1
	
	unknown
};


const std::string & version_to_str(version version_) noexcept;


class status
{
public:
	explicit status(unsigned int code, std::string description) noexcept;
	
	
	inline unsigned int code() const noexcept;
	inline const std::string & code_str() const noexcept;
	inline const std::string & description() const noexcept;
private:
	const unsigned int code_;
	const std::string code_str_,
					  description_;
public:
	
	// Declaration statuses as static class members
	#define SERVER_PROTOCOL_HTTP_CLASS_STATUS
	#include <server/protocol_http_statuses.hpp>
	#undef SERVER_PROTOCOL_HTTP_CLASS_STATUS
};	// class status


};	// namespace http
};	// namespace server


#include <server/protocol_http.hpp>

#endif // SERVER_PROTOCOL_HTTP_H
