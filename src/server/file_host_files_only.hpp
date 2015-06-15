// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <base/mapped_file_exceptions.h>

#include <server/host.h>
#include <server/host_exceptions.h>


template<class FileHost>
std::pair<base::send_buffers_t, base::send_buffers_t>
server::files_only::operator()(const FileHost &host,
							   server::file_host_cache<server::files_only> &cache)
{
	using namespace boost::interprocess;
	using boost::asio::buffer;
	
	
	try {
		cache.mapped_file = std::move(base::mapped_file(cache.path, read_only, MAP_SHARED));
	} catch (const base::path_not_found &e) {
		throw server::path_not_found(cache.path.string());
	} catch (const interprocess_exception &e) {
		switch (e.get_error_code()) {
			case not_such_file_or_directory: case not_found_error:
				throw server::path_not_found(cache.path.string());
			default:
				throw server::path_forbidden(cache.path.string() + ": file_mapping: " + e.what());
		}
	}
	
	
	std::pair<base::send_buffers_t, base::send_buffers_t> res;
	res.second.reserve(1);
	
	
	const void *file_content = cache.mapped_file.data();
	size_t file_size = cache.mapped_file.size();
	
	auto len_it = cache.strings.emplace(cache.strings.end(), std::to_string(file_size));
	
	
	server::host::add_header(res.first, server::http::header_content_length, *len_it);
	server::host::add_buffer(res.second, buffer(file_content, file_size));
	
	return res;
}
