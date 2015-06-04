// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef FILE_HOST_TEMPLATE_PAGES_ONLY_H
#define FILE_HOST_TEMPLATE_PAGES_ONLY_H

#include <string>

#include <boost/filesystem/path.hpp>

#include <server/types.h>
#include <templatizer/page.h>

#include <page_model.h>


class template_pages_only
{
public:
	class cache
	{
	public:
		templatizer::page page;
		std::string content_len_str;
	};	// class cache
	
	
	template_pages_only(page_model &page_model);
	
	
	std::pair<server::send_buffers_t, server::send_buffers_t>
	operator()(const boost::filesystem::path &path,
			   server::file_host_cache<template_pages_only>::raw_ptr_t cache_ptr);
private:
	page_model &page_model_;
};	// template_pages_only


#endif // FILE_HOST_TEMPLATE_PAGES_ONLY_H
