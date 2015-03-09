// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef CLIENT_POOL_H
#define CLIENT_POOL_H

#include <memory>
#include <mongoc.h>

#include "uri.h"

namespace mongo {

class client_pool
{
public:
	client_pool() = delete;
	
	client_pool(const mongo::uri &uri);
	client_pool(const client_pool &other);
	client_pool(client_pool &&other);
	
	
	
private:
	std::shared_ptr<mongoc_client_pool_t> client_pool_;
};

};	// namespace mongo

#endif // CLIENT_POOL_H
