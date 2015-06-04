// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/file_host_files_only.h>

#include <base/mapped_file_exceptions.h>

#include <server/host.h>
#include <server/host_exceptions.h>


std::pair<server::send_buffers_t, server::send_buffers_t>
server::files_only::operator()(const boost::filesystem::path &path,
							   server::file_host_cache<server::files_only>::raw_ptr_t cache_ptr)
{
	using namespace boost::interprocess;
	using boost::asio::buffer;
	
	
	try {
		cache_ptr->mapped_file = std::move(base::mapped_file(path, read_only, MAP_SHARED));
	} catch (const base::path_not_found &e) {
		throw server::path_not_found(path.string());
	} catch (const interprocess_exception &e) {
		switch (e.get_error_code()) {
			case not_such_file_or_directory: case not_found_error:
				throw server::path_not_found(path.string());
			default:
				throw server::path_forbidden(path.string() + ": file_mapping: " + e.what());
		}
	}
	
	
	std::pair<server::send_buffers_t, server::send_buffers_t> res;
	res.second.reserve(1);
	
	
	const void *file_content = cache_ptr->mapped_file.data();
	size_t file_size = cache_ptr->mapped_file.size();
	
	auto len_it = cache_ptr->strings.emplace(cache_ptr->strings.end(), std::to_string(file_size));
	
	
	server::host::add_header(res.first, server::http::header_content_length, *len_it);
	server::host::add_buffer(res.second, buffer(file_content, file_size));
	
	return res;
}
