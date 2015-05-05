// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <file_host_files_and_template_pages.h>


files_and_template_pages::files_and_template_pages(
		page_model &page_model,
		const files_and_template_pages_parameters &parameters):
	server::files_only(),
	template_pages_only(page_model),
	
	parameters_(parameters)
{}


std::pair<server::send_buffers_t, server::send_buffers_t>
files_and_template_pages::operator()(
		const std::string &path,
		server::file_host_cache<files_and_template_pages>::raw_ptr_t cache_ptr)
{
	bool is_template_page = ((this->parameters_.default_behavior
							  == files_and_template_pages_parameters::behavior::template_pages)?
							  true: false);
	
	if (std::regex_match(path, this->parameters_.change_behavior_regex))
		is_template_page = !is_template_page;
	
	
	if (is_template_page) {
		cache_ptr->template_pages_only_ptr =
			std::make_shared<server::file_host_cache<template_pages_only>>();
		
		return this->template_pages_only::operator()(
			path, cache_ptr->template_pages_only_ptr.get());
	} else {
		cache_ptr->files_only_ptr =
			std::make_shared<server::file_host_cache<files_only>>();
		
		return this->server::files_only::operator()(
			path, cache_ptr->files_only_ptr.get());
	}
}
