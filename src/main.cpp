// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <iostream>

#include "mongo/mongoc_handler.h"
#include "mongo/client_pool.h"
#include "mongo/uri.h"

int main()
{
	mongo::mongoc_handler handler;
	
	mongo::client client("mongodb://localhost:27017/");
	mongo::collection collection = client.collection("test", "test");
	
	bson_oid_t oid;
	bson_error_t error;
	bson_t *doc = bson_new();
	bson_oid_init(&oid, nullptr);
	BSON_APPEND_OID(doc, "_id", &oid);
	BSON_APPEND_UTF8(doc, "hello", "world");
	
	if (!collection.insert(doc, MONGOC_INSERT_NONE, nullptr, &error))
		std::cerr << error.message << std::endl;
	
	bson_destroy(doc);
	return 0;
}
