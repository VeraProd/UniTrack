// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include "client_pool.h"

mongo::client_pool::client_pool(const mongo::uri &uri):
	client_pool_(mongoc_client_pool_new(uri.raw()),
				 mongoc_client_pool_destroy)
{}


mongo::client_pool::client_pool(const client_pool &other):
	client_pool_(other.client_pool_)
{}

mongo::client_pool::client_pool(client_pool &&other):
	client_pool_(std::move(other.client_pool_))
{}
