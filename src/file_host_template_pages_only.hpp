// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <stdexcept>

#include <server/host.h>


inline
template_pages_only::template_pages_only(page_model &page_model):
	page_model_(page_model)
{}


template<class FileHost>
std::pair<server::send_buffers_t, server::send_buffers_t>
template_pages_only::operator()(const FileHost &host,
								server::file_host_cache<template_pages_only> &cache)
{
	// Getting the page
	try {										// Try to use cached page
		cache.page_ptr = this->pages_cache_.at(cache.path.string());
	} catch (const std::out_of_range &e) {		// ...or load the new one, if not cached
		cache.page_ptr = std::make_shared<templatizer::page>(cache.path);
		this->pages_cache_.emplace(cache.path.string(), cache.page_ptr);
	}
	
	
	// Generating data to send
	std::pair<server::send_buffers_t, server::send_buffers_t> res;
	size_t content_len = cache.page_ptr->generate(res.second, cache.strings, this->page_model_);
	
	cache.content_len_str = std::move(std::to_string(content_len));
	
	server::host::add_header(res.first, server::http::header_content_length,
										cache.content_len_str);
	
	
	return res;
}
