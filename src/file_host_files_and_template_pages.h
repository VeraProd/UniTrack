// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef FILE_HOST_FILES_AND_TEMPLATE_PAGES_H
#define FILE_HOST_FILES_AND_TEMPLATE_PAGES_H

#include <server/types.h>
#include <server/file_host_files_only.h>

#include <file_host_template_pages_only.h>
#include <file_host_files_and_template_pages_parameters.h>
#include <page_model.h>


class files_and_template_pages:
	private server::files_only,
	private template_pages_only
{
public:
	class cache
	{
	public:
		// Data
		server::file_host_cache<files_only>::unique_ptr_t files_only_ptr;
		server::file_host_cache<template_pages_only>::unique_ptr_t template_pages_only_ptr;
	};	// class cache
	
	
	inline files_and_template_pages(page_model &page_model,
									const files_and_template_pages_parameters &parameters);
	
	
	template<class FileHost>
	std::pair<server::send_buffers_t, server::send_buffers_t>
	operator()(const FileHost &host,
			   server::file_host_cache<files_and_template_pages> &cache);
private:
	files_and_template_pages_parameters parameters_;
};	// files_and_template_pages


#include <file_host_files_and_template_pages.hpp>

#endif // FILE_HOST_FILES_AND_TEMPLATE_PAGES_H
