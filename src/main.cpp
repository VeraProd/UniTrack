// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <iostream>
#include <vector>
#include <unordered_map>
#include <regex>

// #include <mongo/mongoc_handler.h>
// #include <mongo/client_pool.h>
// #include <mongo/document.h>

#include <logger/logger.h>
#include <server/server_http.h>
#include <server/file_host.h>

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
		logger::logger logger(std::clog);
		
		server::file_host_parameters file_host_parameters;
		file_host_parameters.name				= "localhost";
		file_host_parameters.ports				= { 8080 };
		file_host_parameters.allow_match_mode	=
			server::file_host_parameters::allow_match_mode::any;
		file_host_parameters.root				= "www/";
		file_host_parameters.allow_regexes.emplace_back("/.*");
		
		auto file_host_ptr = std::make_shared<server::file_host>(logger, file_host_parameters);
		
		
		server::server_http_parameters server_http_parameters;
		server_http_parameters.ports			= { 8080, 8081 };
		server_http_parameters.workers			= 3;
		
		server::server_http server(logger, server_http_parameters);
		
		server.hosts_manager().add_host(file_host_ptr);
		
		
		// Waiting for Ctrl+D
		while (std::cin) std::cin.get();
		
		
		logger.stream(logger::level::info)
			<< "Main: Stopping server...";
		
		server.stop();
	} catch (...) {
		std::cerr << "Exception catched!" << std::endl;
	}
	
	return 0;
}
