// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef FILE_HOST_TEMPLATE_PAGES_ONLY_H
#define FILE_HOST_TEMPLATE_PAGES_ONLY_H

#include <string>
#include <memory>
#include <unordered_map>

#include <base/buffer.h>
#include <templatizer/page.h>

#include <page_model.h>


class template_pages_only
{
public:
	class cache
	{
	public:
		// Data
		std::shared_ptr<templatizer::page> page_ptr;
		std::string content_len_str;
	};	// class cache
	
	
	inline template_pages_only(page_model &page_model);
	
	
	template<class FileHost>
	std::pair<base::send_buffers_t, base::send_buffers_t>
	operator()(const FileHost &host,
			   server::file_host_cache<template_pages_only> &cache);
private:
	std::unordered_map<std::string, std::shared_ptr<templatizer::page>> pages_cache_;
	
	page_model &page_model_;
};	// template_pages_only


#include <file_host_template_pages_only.hpp>

#endif // FILE_HOST_TEMPLATE_PAGES_ONLY_H
