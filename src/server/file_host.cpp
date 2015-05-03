// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/file_host.h>

#include <regex>

#include <server/host_exceptions.h>


server::file_host::file_host(logger::logger &logger,
							 const file_host_parameters &parameters):
	server::host(logger, static_cast<const server::host_parameters &>(parameters)),
	
	file_host_parameters_(static_cast<const server::file_host_only_parameters &>(parameters))
{}


// virtual
std::pair<server::host::send_buffers_t, server::host::cache_ptr_t>
server::file_host::response(std::string &&uri,
							server::http::method method,
							server::http::version version,
							server::headers_t &&request_headers,
							server::headers_t &&response_headers)
{
	if (!this->validate_method(method)) {
		this->logger().stream(logger::level::sec_warning)
			<< "File host: Host \"" << this->name()
			<< "\": Requested unallowed method: \"" << server::http::method_to_str(method)
			<< "\" => " << server::http::status::method_not_allowed.code_str() << '.';
		
		return std::move(this->phony_response(
			version,
			server::http::status::method_not_allowed,
			std::move(response_headers),
			{
				{
					server::http::header_allow,
					server::http::method_to_str(server::http::method::GET)
				}
			}));
	}
	
	if (!this->validate_uri(uri)) {
		this->logger().stream(logger::level::sec_warning)
			<< "File host: Host \"" << this->name()
			<< "\": Requested unallowed URI: \"" << uri
			<< "\" => " << server::http::status::forbidden.code_str() << '.';
		
		return std::move(this->phony_response(version,
											  server::http::status::forbidden,
											  std::move(response_headers)));
	}
	
	
	// Headers must NOT contain "Content-Length"! (Let it throw! :) )
	server::host::validate_headers(response_headers);
	
	
	// Processing
	server::file_host::cache_ptr_t cache_ptr;
	cache_ptr = std::make_shared<server::file_host::cache>();
	cache_ptr->uri = std::move(uri);
	cache_ptr->response_headers = std::move(response_headers);
	
	{
		static const std::regex slash_regex("/+", std::regex::optimize);
		static const std::string single_slash = "/";
		
		using namespace boost::interprocess;
		
		
		// Fixing doubleslashes
		std::string file_name = std::regex_replace(this->file_host_parameters_.root
												   + '/' + cache_ptr->uri,
												   slash_regex, single_slash);
		
		try {
			cache_ptr->file_mapping = std::move(file_mapping(file_name.c_str(),
															 read_only));
			cache_ptr->mapped_region = std::move(mapped_region(cache_ptr->file_mapping,
															   read_only));
		} catch (const interprocess_exception &e) {
			this->logger().stream(logger::level::error)
				<< "File host: Can't map file \"" << file_name << "\": " << e.what() << '.';
			
			const server::http::status *status = nullptr;
			switch (e.get_error_code()) {
				case not_such_file_or_directory: case not_found_error:
					status = &server::http::status::not_found;
					break;
				default:
					status = &server::http::status::forbidden;
					break;
			}
			
			return std::move(this->phony_response(version,
												  *status,
												  std::move(cache_ptr->response_headers)));
		}
	}
	
	
	auto server_name = this->server_name(cache_ptr->response_headers, {});
	
	
	server::host::send_buffers_t res;
	res.reserve(4 + 4 * (cache_ptr->response_headers.size()
						 + ((server_name.second)? 1: 0)));
	
	server::host::add_start_string(res, version, server::http::status::ok);
	
	if (server_name.second)
		server::host::add_header(res, server::http::header_server, *server_name.first);
	server::host::add_headers(res, cache_ptr->response_headers);
	
	
	// Content-Length and content
	{
		using boost::asio::buffer;
		
		const void *file_content = cache_ptr->mapped_region.get_address();
		size_t file_size = cache_ptr->mapped_region.get_size();
		
		auto len_it = cache_ptr->strings.emplace(cache_ptr->strings.end(), std::to_string(file_size));
		server::host::add_header(res, server::http::header_content_length, *len_it);
		server::host::finish_headers(res);
		
		// Content
		server::host::add_buffer(res, buffer(file_content, file_size));
	}
	
	
	return { std::move(res), std::move(cache_ptr) };
}


bool
server::file_host::validate_uri(const std::string &uri) const noexcept
{
	// Checks for denied regexes
	for (const auto &deny_regex: this->file_host_parameters_.deny_regexes)
		if (std::regex_match(uri, deny_regex)) {
			this->logger().stream(logger::level::sec_warning)
				<< "File host: Host \"" << this->name()
				<< "\": Requested denied URI: \"" << uri << "\".";
			return false;
		}
	
	
	// Checks for allowed regexes
	bool verification_status = false;
	
	if (this->file_host_parameters_.allow_regexes.empty()) {
		verification_status = false;
	} else if (this->file_host_parameters_.allow_match_mode
		== file_host_only_parameters::allow_match_mode::any) {	// Any
		verification_status = false;
		for (const auto &allow_regex: this->file_host_parameters_.allow_regexes)
			if (std::regex_match(uri, allow_regex)) {
				verification_status = true;
				break;
			}
	} else {													// All
		verification_status = true;
		for (const auto &allow_regex: this->file_host_parameters_.allow_regexes)
			if (!std::regex_match(uri, allow_regex)) {
				verification_status = false;
				break;
			}
	}
	
	
	if (verification_status == true) {
		this->logger().stream(logger::level::sec_info)
			<< "File host: Host \"" << this->name()
			<< "\": Requested allowed URI: \"" << uri << "\".";
	} else {
		this->logger().stream(logger::level::sec_warning)
			<< "File host: Host \"" << this->name()
			<< "\": Requested unallowed URI: \"" << uri << "\".";
	}
	
	return verification_status;
}


bool
server::file_host::validate_method(server::http::method method) const noexcept
{
	// This host only supports GET method.
	if (method == server::http::method::GET)
		return true;
	return false;
}
