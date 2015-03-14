// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include "uri.h"

mongo::uri::uri(const std::string &uri_string) noexcept:
	uri_(mongoc_uri_new(uri_string.c_str()),
		 mongoc_uri_destroy)
{}

mongo::uri::uri(const uri &other) noexcept:
	uri_(mongoc_uri_copy(other.raw()),
		 mongoc_uri_destroy)
{}

mongo::uri::uri(uri &&other) noexcept:
	uri_(std::move(other.uri_))
{}


mongo::uri &
mongo::uri::operator=(const uri &other) noexcept
{ return *this = std::move(uri(other)); }

mongo::uri &
mongo::uri::operator=(uri &&other) noexcept
{
	this->uri_ = std::move(other.uri_);
	return *this;
}
