// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol_http.h>

#include <server/protocol_http_statuses.hpp>


const std::string
	server::http::space_str					= " ",
	server::http::newline_str				= "\r\n",
	server::http::HTTP_str					= "HTTP",
	server::http::slash_str					= "/",
	server::http::header_separator_str		= ": ",
	
	server::http::header_content_length		= "Content-Length",
	server::http::header_server				= "Server",
	server::http::header_allow				= "Allow";


namespace {

const std::string
	method_GET("GET"),
	method_HEAD("HEAD"),
	method_POST("POST"),
	method_DELETE("DELETE"),
	method_unknown("unknown");

const std::string
	version_1_1("1.1"),
	version_unknown("unknown");

};	// namespace


const std::string &
server::http::method_to_str(method method_) noexcept
{
	switch (method_) {
		case server::http::method::GET:					return method_GET;
		case server::http::method::HEAD:				return method_HEAD;
		case server::http::method::POST:				return method_POST;
		case server::http::method::DELETE:				return method_DELETE;
		case server::http::method::unknown: default:	return method_unknown;
	}
}


const std::string &
server::http::version_to_str(version version_) noexcept
{
	switch (version_) {
		case server::http::version::_1_1:				return version_1_1;
		case server::http::version::unknown: default:	return version_unknown;
	}
}


server::http::status::status(unsigned int code, std::string description) noexcept:
	code_(code),
	code_str_(std::move(std::to_string(code_))),
	description_(std::move(description))
{}
