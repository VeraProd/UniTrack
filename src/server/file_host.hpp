// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <regex>

#include <server/host_exceptions.h>


template<class HostType, class CacheType>
server::file_host<HostType, CacheType>::file_host(logger::logger &logger,
							 const server::file_host_parameters &parameters,
							 HostType &&file_handler):
	server::host(logger, static_cast<const server::host_parameters &>(parameters)),
	
	file_host_parameters_(static_cast<const server::file_host_only_parameters &>(parameters)),
	
	file_handler_(std::move(file_handler))
{}


template<class HostType, class CacheType>
server::file_host<HostType, CacheType>::file_host(logger::logger &logger,
							 const server::file_host_parameters &parameters,
							 const HostType &file_handler):
	server::host(logger, static_cast<const server::host_parameters &>(parameters)),
	
	file_host_parameters_(static_cast<const server::file_host_only_parameters &>(parameters)),
	
	file_handler_(file_handler)
{}


template<class HostType, class CacheType>
// virtual
std::pair<server::send_buffers_t, server::host_cache::ptr_t>
server::file_host<HostType, CacheType>::response(std::string &&uri,
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
	server::file_host<HostType, CacheType>::cache_ptr_t cache_ptr;
	cache_ptr = std::make_shared<server::file_host<HostType, CacheType>::cache_t>();
	cache_ptr->uri = std::move(uri);
	cache_ptr->response_headers = std::move(response_headers);
	
	
	std::pair<server::send_buffers_t, server::send_buffers_t> file_data;
	{
		static const std::regex slash_regex("/+", std::regex::optimize);
		static const std::string single_slash = "/";
		
		
		// Fixing doubleslashes
		std::string path = std::regex_replace(this->file_host_parameters_.root
											  + '/' + cache_ptr->uri,
											  slash_regex, single_slash);
		
		try {
			file_data = std::move(this->file_handler_(path, cache_ptr));
		} catch (const server::path_forbidden &e) {
			this->logger().stream(logger::level::error)
				<< "File host: Can't send path: \"" << path << "\": " << e.what()
				<< " => " << server::http::status::forbidden.code_str() << '.';
			
			return std::move(this->phony_response(version,
												  server::http::status::forbidden,
												  std::move(cache_ptr->response_headers)));
		} catch (const server::path_not_found &e) {
			this->logger().stream(logger::level::error)
				<< "File host: Can't send path: \"" << path << "\": " << e.what()
				<< " => " << server::http::status::not_found.code_str() << '.';
			
			return std::move(this->phony_response(version,
												  server::http::status::not_found,
												  std::move(cache_ptr->response_headers)));
		} catch (...) {
			this->logger().stream(logger::level::error)
				<< "File host: Can't send path: \"" << path << "\": Unknown error"
				<< " => " << server::http::status::internal_server_error.code_str() << '.';
			
			return std::move(this->phony_response(version,
												  server::http::status::internal_server_error,
												  std::move(cache_ptr->response_headers)));
		}
	}
	
	
	auto server_name = this->server_name(cache_ptr->response_headers, {});
	
	
	server::send_buffers_t res;
	res.reserve(4 + 4 * (cache_ptr->response_headers.size()
						 + file_data.first.size() + file_data.second.size()
						 + ((server_name.second)? 1: 0)));
	
	server::host::add_start_string(res, version, server::http::status::ok);
	
	if (server_name.second)
		server::host::add_header(res, server::http::header_server, *server_name.first);
	server::host::add_headers(res, cache_ptr->response_headers);
	
	
	res.insert(res.end(), file_data.first.begin(), file_data.first.end());
	server::host::finish_headers(res);
	
	res.insert(res.end(), file_data.second.begin(), file_data.second.end());
	
	return { std::move(res), std::move(cache_ptr) };
}


template<class HostType, class CacheType>
bool
server::file_host<HostType, CacheType>::validate_uri(const std::string &uri) const noexcept
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
	} else if (this->file_host_parameters_.mode
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


template<class HostType, class CacheType>
bool
server::file_host<HostType, CacheType>::validate_method(server::http::method method) const noexcept
{
	// This host only supports GET method.
	if (method == server::http::method::GET)
		return true;
	return false;
}
