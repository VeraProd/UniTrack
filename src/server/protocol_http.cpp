// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol_http.h>

#include <server/protocol_http_statuses.hpp>


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

};


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