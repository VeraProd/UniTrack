// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef INTERFACE_MANAGER_H
#define INTERFACE_MANAGER_H

#include <string>
#include <memory>

#include <logger/logger.h>
#include <server/server_http.h>

#include <page_model.h>


class interface_manager
{
public:
	interface_manager(logger::logger &logger,
					  const std::string &config_file_path,
					  page_model &model);
	
	// Non-copy/move constructable/assignable.
	interface_manager(const interface_manager &other) = delete;
	interface_manager(interface_manager &&other);
	interface_manager & operator=(const interface_manager &other) = delete;
	interface_manager & operator=(interface_manager &&other);
	
	
	void stop();
private:
	logger::logger &logger_;
	
	page_model &model_;
	
	std::unique_ptr<server::server_http> server_ptr_;
};	// class interface_manager


#endif // INTERFACE_MANAGER_H
