// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include "client.h"

mongo::client::client(mongoc_client_t *new_client, deleter_t d) noexcept:
	client_(new_client, std::move(d))
{}

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


mongo::collection
mongo::client::collection(const std::string &db_name, const std::string &collention_name)
{
	return mongo::collection(mongoc_client_get_collection(this->client_.get(),
														  db_name.c_str(),
														  collention_name.c_str()));
}
