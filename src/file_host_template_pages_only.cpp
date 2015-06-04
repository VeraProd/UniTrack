// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <file_host_template_pages_only.h>

#include <server/host.h>


template_pages_only::template_pages_only(page_model &page_model):
	page_model_(page_model)
{}


std::pair<server::send_buffers_t, server::send_buffers_t>
template_pages_only::operator()(const boost::filesystem::path &path,
								server::file_host_cache<template_pages_only>::raw_ptr_t cache_ptr)
{
	cache_ptr->page.load(path);
	
	std::pair<server::send_buffers_t, server::send_buffers_t> res;
	size_t content_len = cache_ptr->page.generate(res.second, this->page_model_);
	
	cache_ptr->content_len_str = std::move(std::to_string(content_len));
	
	server::host::add_header(res.first, server::http::header_content_length,
										cache_ptr->content_len_str);
	
	return res;
}
