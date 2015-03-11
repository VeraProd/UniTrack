// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <functional>
#include <memory>
#include <mongoc.h>

#include "uri.h"

namespace mongo {

class client
{
public:
	template<class Deleter>
	client(mongoc_client_t *new_client, Deleter d) noexcept:
		client_(new_client, std::move(d))
	{}
	
	client(const std::string &uri) noexcept;
	client(const mongo::uri &uri) noexcept;
	
	client(const client &other) = delete;
	client(client &&other);
	
	client & operator=(const client &other) = delete;
	client & operator=(client &&other);
	
	
private:
	std::unique_ptr<mongoc_client_t, std::function<void (mongoc_client_t *)>> client_;
};

}	// namespace mongo

#endif // CLIENT_H
