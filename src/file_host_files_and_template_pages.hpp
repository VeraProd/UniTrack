// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <string>
#include <regex>


inline
files_and_template_pages::files_and_template_pages(
		page_model &page_model,
		const files_and_template_pages_parameters &parameters):
	server::files_only(),
	template_pages_only(page_model),
	
	parameters_(parameters)
{}


template<class FileHost>
std::pair<base::send_buffers_t, base::send_buffers_t>
files_and_template_pages::operator()(const FileHost &host,
									 server::file_host_cache<files_and_template_pages> &cache)
{
	bool is_template_page = ((this->parameters_.default_behavior
							  == files_and_template_pages_parameters::behavior::template_pages)?
							  true: false);
	
	if (std::regex_match(cache.path.string(), this->parameters_.change_behavior_regex))
		is_template_page = !is_template_page;
	
	
	if (is_template_page) {
		cache.template_pages_only_ptr
			= std::make_unique<server::file_host_cache<template_pages_only>>(std::move(cache));
		
		return this->template_pages_only::operator()(host, *cache.template_pages_only_ptr);
	} else {
		cache.files_only_ptr
			= std::make_unique<server::file_host_cache<files_only>>(std::move(cache));
		
		return this->server::files_only::operator()(host, *cache.files_only_ptr);
	}
}
