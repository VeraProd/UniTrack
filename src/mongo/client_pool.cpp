// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <mongo/client_pool.h>

mongo::client_pool::client_pool(const mongo::uri &uri) noexcept:
	client_pool_(mongoc_client_pool_new(uri.raw()),
				 mongoc_client_pool_destroy)
{}

mongo::client_pool::client_pool(const client_pool &other) noexcept:
	client_pool_(other.client_pool_)
{}

mongo::client_pool::client_pool(client_pool &&other) noexcept:
	client_pool_(std::move(other.client_pool_))
{}


mongo::client
mongo::client_pool::pop() noexcept
{
	return mongo::client(mongoc_client_pool_pop(this->client_pool_.get()),
						 this->client_pusher_);
}

mongo::client
mongo::client_pool::try_pop()
{
	mongoc_client_t *client = mongoc_client_pool_try_pop(this->client_pool_.get());
	if (client == nullptr)
		throw mongo::client_pool::cant_pop();
	return mongo::client(client, this->client_pusher_);
}


void
mongo::client_pool::push(mongo::client &&) noexcept
{}	// Just ignoge argument -- it will be pushed automatically when destroyed
