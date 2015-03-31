// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <iostream>
#include <vector>

// #include "mongo/mongoc_handler.h"
// #include "mongo/client_pool.h"
// #include "mongo/document.h"

#include <Wt/WServer>

#include "application.h"

int main(int argc, char **argv)
{
	// mongo::mongoc_handler handler;
	
	// mongo::client client("mongodb://localhost:27017/");
	// mongo::collection collection = client.collection("test", "test");
	
	// bson_error_t error;
	
	// mongo::oid oid;
	// mongo::document doc;
	// doc.append()/*("_id", oid)*/("hello", "world");
	
	// std::cout << "Document: " << doc.json() << std::endl;
	
	// if (!collection.insert(doc, &error))
	// 	std::cerr << error.message << std::endl;
	
	
	return Wt::WRun(argc, argv, Application::create);
}
