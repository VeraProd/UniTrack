// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <iostream>
#include <vector>
#include <unordered_map>

#include <boost/asio.hpp>

// #include <mongo/mongoc_handler.h>
// #include <mongo/client_pool.h>
// #include <mongo/document.h>

#include <logger/logger.h>
#include <server/server_http.h>

#include <templatizer/page.h>
#include <templatizer/model.h>


class model:
	public templatizer::model,
	public std::unordered_map<std::string, std::string>
{
public:
	virtual const std::string & at(const std::string &var_name) const override
	{
		try {
			return this->std::unordered_map<std::string, std::string>::at(var_name);
		} catch (...) {
			std::cerr << "Requested unknown variable: \"" << var_name << "\"." << std::endl;
			return this->unknown_;
		}
	}
private:
	std::string unknown_ = "!!!UNKNOWN_VAR!!!";
};


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
	
	// templatizer::page page("www/profile.html");
	
	// model m;
	// m.emplace("USERNAME", "Dmitry Kukovinets");
	// m.emplace("UNIVERSITY", "STANKIN");
	// m.emplace("DEPARTMENT", "Inteh");
	// m.emplace("TAGS", "Student");
	
	// std::cout << std::endl << "Generated page:" << std::endl
	// 		  << page(m) << std::endl;
	
	try {
	logger::logger logger(std::cerr);
	
	server::server_parameters parameters;
	parameters.port = 8080;
	parameters.workers = 1;
	
	server::server_http server(logger, parameters);
	
	boost::asio::io_service io_service;
	boost::asio::signal_set signal_set(io_service, SIGINT, SIGTERM, SIGQUIT);
	
	signal_set.async_wait(
		[&server, &logger] (const boost::system::error_code &err, int signal_number) {
			if (err) {
				logger.stream(logger::level::error)
					<< "Main: Caught exception (signal: " << signal_number << "): " << err.message();
			} else {
				logger.stream(logger::level::info)
					<< "Main: Stopping server (signal: " << signal_number << ")...";
				server.stop();
			}
		});
	
	// io_service.run();
	
	while (std::cin) std::cin.get();
	
	logger.stream(logger::level::info)
		<< "Main: Stopping server...";
	server.stop();
	
	sleep(5);
	
	server.join();
	} catch (...) {
		std::cerr << "Caught! " << std::endl;
	}
	return 0;
}
