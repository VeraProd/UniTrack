// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <interface_manager.h>

#include <fstream>
#include <stdexcept>

#include <json.hpp>

#include <templatizer/page.h>

#include <server/server_http.h>
#include <server/file_host.h>
#include <server/file_host_files_only.h>

#include <file_host_template_pages_only.h>
#include <file_host_files_and_template_pages.h>

#include <json_utils.h>


interface_manager::interface_manager(logger::logger &logger,
									 const std::string &config_file_path,
									 page_model &model):
	logger_(logger),
	
	model_(model)
{
	try {
		nlohmann::json config = std::move(utils::json_from_file(config_file_path));
		this->logger_.stream(logger::level::info)
			<< "Interface manager: Read config: \"" << config_file_path << "\".";
		
		
		// Server parameters
		{
			const auto &server = config.at("server_http");
			server::server_http_parameters params;
			params.ports = server.at("ports").get<decltype(params.ports)>();
			params.workers = server.at("workers").get<decltype(params.workers)>();
			
			this->server_ptr_.reset(new server::server_http(logger, params));
		}
		
		
		// Hosts parameters
		{
			const auto &host_list_config = config.at("file_hosts");
			for (const auto &host_config: host_list_config) {
				server::file_host_parameters params(host_config);
				
				
				{
					const auto &type = host_config.at("type").get<std::string>();
					if (type == "files_only") {
						this->server_ptr_->host_manager().add_host(
							std::make_shared<server::file_host<server::files_only>>(
								this->logger_,
								params
							)
						);
					} else if (type == "template_pages_only") {
						this->server_ptr_->host_manager().add_host(
							std::make_shared<server::file_host<template_pages_only>>(
								this->logger_,
								params,
								this->model_
							)
						);
					} else if (type == "files_and_template_pages") {
						files_and_template_pages_parameters additional_params(host_config);
						
						this->server_ptr_->host_manager().add_host(
							std::make_shared<server::file_host<files_and_template_pages>>(
								this->logger_,
								params,
								files_and_template_pages(
									this->model_,
									additional_params
								)
							)
						);
					} else {
						throw std::logic_error("Incorrect config");
					}
				}
			}
		}
	} catch (const std::exception &e) {
		this->logger_.stream(logger::level::error)
			<< "Interface manager: " << e.what() << '.';
	}
}


void
interface_manager::stop()
{
	if (this->server_ptr_)
		this->server_ptr_->stop();
}
