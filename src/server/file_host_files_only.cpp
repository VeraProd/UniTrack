// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/file_host_files_only.h>

#include <server/host.h>
#include <server/host_exceptions.h>


namespace {

const std::string zero_content_length_str = "0";

};	// namespace


std::pair<server::send_buffers_t, server::send_buffers_t>
server::files_only::operator()(const std::string &path,
							   server::file_host_cache<server::files_only>::ptr_t cache_ptr)
{
	using namespace boost::interprocess;
	using boost::asio::buffer;
	
	
	try {
		cache_ptr->file_mapping = std::move(file_mapping(path.c_str(), read_only));
		cache_ptr->mapped_region = std::move(mapped_region(cache_ptr->file_mapping, read_only,
														   0, 0, nullptr, MAP_SHARED));
	} catch (const interprocess_exception &e) {
		switch (e.get_error_code()) {
			case not_such_file_or_directory: case not_found_error:
				throw server::path_not_found(path);
			case invalid_argument: {	// Thrown by mapped_region, when the file is empty
				server::send_buffers_t headers;
				server::host::add_header(headers, server::http::header_content_length,
												  zero_content_length_str);
				return { std::move(headers), {} };
			}
			default:
				throw server::path_forbidden(path + std::string(": file_mapping: ") + e.what());
		}
	}
	
	
	std::pair<server::send_buffers_t, server::send_buffers_t> res;
	res.second.reserve(1);
	
	
	const void *file_content = cache_ptr->mapped_region.get_address();
	size_t file_size = cache_ptr->mapped_region.get_size();
	
	auto len_it = cache_ptr->strings.emplace(cache_ptr->strings.end(), std::to_string(file_size));
	
	
	server::host::add_header(res.first, server::http::header_content_length, *len_it);
	server::host::add_buffer(res.second, buffer(file_content, file_size));
	
	return res;
}
