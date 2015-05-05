// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef FILE_HOST_FILES_AND_TEMPLATE_PAGES_H
#define FILE_HOST_FILES_AND_TEMPLATE_PAGES_H

#include <string>
#include <regex>

#include <server/types.h>
#include <server/file_host_files_only.h>
#include <file_host_template_pages_only.h>
#include <page_model.h>


struct files_and_template_pages_parameters
{
	enum class behavior {
		template_pages,
		files
	};
	
	
	// By default: interpret all files as plain files...
	behavior default_behavior = behavior::files;
	
	// ...but change behavior for .html files.
	std::regex change_behavior_regex = std::regex(".*\\.html");
};	// struct files_and_template_pages_parameters


class files_and_template_pages:
	private server::files_only,
	private template_pages_only
{
public:
	class cache
	{
	public:
		server::file_host_cache<files_only>::ptr_t files_only_ptr;
		server::file_host_cache<template_pages_only>::ptr_t template_pages_only_ptr;
	};	// class cache
	
	
	files_and_template_pages(page_model &page_model,
							 const files_and_template_pages_parameters &parameters);
	
	
	std::pair<server::send_buffers_t, server::send_buffers_t>
	operator()(const std::string &path,
			   server::file_host_cache<files_and_template_pages>::raw_ptr_t cache_ptr);
private:
	files_and_template_pages_parameters parameters_;
};	// files_and_template_pages


#endif // FILE_HOST_FILES_AND_TEMPLATE_PAGES_H
