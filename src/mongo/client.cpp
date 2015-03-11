// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include "client.h"

mongo::client::client(const std::string &uri) noexcept:
	client_(mongoc_client_new(uri.c_str()), mongoc_client_destroy)
{}

mongo::client::client(const mongo::uri &uri) noexcept:
	client_(mongoc_client_new_from_uri(uri.raw()), mongoc_client_destroy)
{}

mongo::client::client(mongo::client &&other):
	client_(std::move(other.client_))
{}


mongo::client &
mongo::client::operator=(mongo::client &&other)
{
	this->client_ = std::move(other.client_);
	return *this;
}
