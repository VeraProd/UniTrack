// Author: Dmitry Kukovinets (d1021976@gmail.com)

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
	cache.page.load(cache.path);
	
	std::pair<server::send_buffers_t, server::send_buffers_t> res;
	size_t content_len = cache.page.generate(res.second, this->page_model_);
	
	cache.content_len_str = std::move(std::to_string(content_len));
	
	server::host::add_header(res.first, server::http::header_content_length,
										cache.content_len_str);
	
	return res;
}
