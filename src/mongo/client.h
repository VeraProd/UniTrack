// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <functional>
#include <memory>
#include <mongoc.h>

#include "uri.h"
#include "collection.h"

namespace mongo {

class client
{
public:
	typedef std::function<void (mongoc_client_t *)> deleter_t;
	
	
	client(mongoc_client_t *new_client, deleter_t d) noexcept;
	client(const std::string &uri) noexcept;
	client(const mongo::uri &uri) noexcept;
	
	client(const client &other) = delete;
	client(client &&other);
	
	client & operator=(const client &other) = delete;
	client & operator=(client &&other);
	
	
	mongo::collection collection(const std::string &db_name, const std::string &collention_name);
private:
	std::unique_ptr<mongoc_client_t, deleter_t> client_;
};

}	// namespace mongo

#endif // CLIENT_H
