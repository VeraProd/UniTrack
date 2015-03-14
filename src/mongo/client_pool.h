// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef CLIENT_POOL_H
#define CLIENT_POOL_H

#include <functional>
#include <memory>
#include <stdexcept>
#include <mongoc.h>

#include "uri.h"
#include "client.h"

namespace mongo {

class client_pool
{
public:
	class cant_pop: public std::runtime_error
	{
	public:
		cant_pop(): std::runtime_error("Can't pop client from client pool.") {}
	};
	
	
	client_pool() = delete;
	
	client_pool(const mongo::uri &uri) noexcept;
	client_pool(const client_pool &other) noexcept;
	client_pool(client_pool &&other) noexcept;
	
	mongo::client pop() noexcept;
	mongo::client try_pop();
	
	void push(mongo::client &&) noexcept;
private:
	std::shared_ptr<mongoc_client_pool_t> client_pool_;
	
	const std::function<void (mongoc_client_t *)> client_pusher_ =
		[this] (mongoc_client_t *client) {
			mongoc_client_pool_push(this->client_pool_.get(), client);
		};
};

};	// namespace mongo

#endif // CLIENT_POOL_H
