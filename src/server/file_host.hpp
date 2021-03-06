// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <regex>

#include <boost/filesystem.hpp>

#include <base/mapped_file_exceptions.h>

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
server::response_data_t
server::file_host<HostType, CacheType>::response(std::string &&uri,
												 server::http::method method,
												 server::http::version version,
												 server::headers_t &&request_headers,
												 server::headers_t &&response_headers)
{
	// Headers must NOT contain "Content-Length"! (Let it throw! :) )
	server::host::validate_headers(response_headers);
	
	
	if (!this->validate_method(method)) {
		this->logger().stream(logger::level::sec_warning)
			<< "File host: Host \"" << this->name()
			<< "\": Requested unallowed method: \"" << server::http::method_to_str(method)
			<< "\" => " << server::http::status::method_not_allowed.code_str() << '.';
		
		return this->phony_response(
			version,
			server::http::status::method_not_allowed,
			std::move(response_headers),
			{
				{
					server::http::header_allow,
					server::http::method_to_str(server::http::method::GET)
				}
			});
	}
	
	
	// Filling cache
	server::file_host<HostType, CacheType>::cache_shared_ptr_t cache_ptr
		= std::make_shared<server::file_host<HostType, CacheType>::cache_t>();
	
	
	// Parsing URI
	try {
		if (!this->parse_uri(uri, *cache_ptr)) {
			this->logger().stream(logger::level::sec_warning)
				<< "File host: Host \"" << this->name()
				<< "\": Incorrect URI: \"" << uri
				<< "\" => " << server::http::status::bad_request.code_str() << '.';
			
			return this->phony_response(version,
										server::http::status::bad_request,
										std::move(response_headers));
		}
	} catch (const boost::filesystem::filesystem_error &e) {
		return this->handle_filesystem_error(
			e, version, std::move(cache_ptr),
			server::http::status::forbidden
		);
	}
	
	
	if (!this->validate_args(cache_ptr->args_map, cache_ptr->args_set)) {
		auto rec_obj = std::move(this->logger().stream(logger::level::sec_warning)
			<< "File host: Host \"" << this->name()
			<< "\": Requested unallowed args: ");
		
		for (const auto &p: cache_ptr->args_map)
			rec_obj << " [" << p.first << "=" << p.second << ']';
		
		for (const auto &arg: cache_ptr->args_set)
			rec_obj << " [" << arg << ']';
		
		rec_obj << " => " << server::http::status::forbidden.code_str() << '.';
		
		
		return this->phony_response(version,
									server::http::status::forbidden,
									std::move(response_headers));
	}
	
	
	cache_ptr->response_headers = std::move(response_headers);
	
	
	return this->response(std::move(cache_ptr),
						  method, version,
						  std::move(request_headers));
}


// private:
template<class HostType, class CacheType>
// virtual
server::response_data_t
server::file_host<HostType, CacheType>::response(
	server::file_host<HostType, CacheType>::cache_shared_ptr_t &&cache_ptr,
	server::http::method method,
	server::http::version version,
	server::headers_t &&request_headers)
{
	using namespace boost::filesystem;
	
	
	if (!this->validate_path(cache_ptr->path.string())) {
		this->logger().stream(logger::level::sec_warning)
			<< "File host: Host \"" << this->name()
			<< "\": Requested unallowed path: " << cache_ptr->path
			<< " => " << server::http::status::forbidden.code_str() << '.';
		
		return this->phony_response(version,
									server::http::status::forbidden,
									std::move(cache_ptr->response_headers));
	}
	
	
	std::pair<base::send_buffers_t, base::send_buffers_t> file_data;
	
	try {
		// The path is relative, see parse_uri(): "./index.html"
		// Or absolute: "/var/unitrack/www/index.html"
		cache_ptr->path = canonical(cache_ptr->path, this->file_host_parameters_.root);
		
		file_data = std::move(this->file_handler_(*this, *cache_ptr));
	} catch (const base::path_is_directory &) {
		auto &p = cache_ptr->path;
		
		auto rec_obj = std::move(this->logger().stream(logger::level::info)
		<< "File host: Host \"" << this->name()
		<< "\": URI points to directory: \"" << p << "\"");
		
		// Append "/index.html"
		p /= "index.html";
		
		rec_obj << " => Try \"" << p << "\".";
		
		
		return this->response(std::move(cache_ptr),
							  method, version,
							  std::move(request_headers));
	} catch (const boost::filesystem::filesystem_error &e) {
		return this->handle_filesystem_error(
			e, version, std::move(cache_ptr),
			server::http::status::forbidden
		);
	} catch (const server::path_forbidden &e) {
		return this->log_and_phony_response(
			e.what(), version, std::move(cache_ptr),
			server::http::status::forbidden
		);
	} catch (const server::path_not_found &e) {
		return this->log_and_phony_response(
			e.what(), version, std::move(cache_ptr),
			server::http::status::not_found
		);
	} catch (...) {
		return this->log_and_phony_response(
			"Unknown error", version, std::move(cache_ptr),
			server::http::status::internal_server_error
		);
	}
	
	
	auto server_name = this->server_name(cache_ptr->response_headers, {});
	
	
	base::send_buffers_t res;
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
server::file_host<HostType, CacheType>::validate_path(const std::string &path) const noexcept
{
	// Checks for denied regexes
	for (const auto &deny_regex: this->file_host_parameters_.deny_regexes)
		if (std::regex_match(path, deny_regex)) {
			this->logger().stream(logger::level::sec_warning)
				<< "File host: Host \"" << this->name()
				<< "\": Requested denied path: \"" << path << "\".";
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
			if (std::regex_match(path, allow_regex)) {
				verification_status = true;
				break;
			}
	} else {													// All
		verification_status = true;
		for (const auto &allow_regex: this->file_host_parameters_.allow_regexes)
			if (!std::regex_match(path, allow_regex)) {
				verification_status = false;
				break;
			}
	}
	
	
	if (verification_status == true) {
		this->logger().stream(logger::level::sec_info)
			<< "File host: Host \"" << this->name()
			<< "\": Requested allowed path: \"" << path << "\".";
	} else {
		this->logger().stream(logger::level::sec_warning)
			<< "File host: Host \"" << this->name()
			<< "\": Requested unallowed path: \"" << path << "\".";
	}
	
	return verification_status;
}


template<class HostType, class CacheType>
bool
server::file_host<HostType, CacheType>::validate_args(
	const server::uri_arguments_map_t &args_map,
	const server::uri_arguments_set_t &args_set) const noexcept
{
	return true;
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


template<class HostType, class CacheType>
server::response_data_t
server::file_host<HostType, CacheType>::log_and_phony_response(
	const std::string &message,
	server::http::version version,
	server::file_host<HostType, CacheType>::cache_shared_ptr_t cache_ptr,
	const server::http::status &status)
{
	this->logger().stream(logger::level::error)
		<< "File host: Host \"" << this->name()
		<< "\": Can't send path: " << cache_ptr->path << ": " << message
		<< " => " << status.code_str() << '.';
	
	return this->phony_response(version,
								status,
								std::move(cache_ptr->response_headers));
}


template<class HostType, class CacheType>
server::response_data_t
server::file_host<HostType, CacheType>::handle_filesystem_error(
	const boost::filesystem::filesystem_error &e,
	server::http::version version,
	server::file_host<HostType, CacheType>::cache_shared_ptr_t cache_ptr,
	const server::http::status &status)
{
	switch (e.code().value()) {
		case boost::system::errc::no_such_file_or_directory:
			return this->log_and_phony_response(
				e.code().message(), version, std::move(cache_ptr),
				server::http::status::not_found
			);
		case boost::system::errc::permission_denied:
			return this->log_and_phony_response(
				e.code().message(), version, std::move(cache_ptr),
				server::http::status::forbidden
			);
		default:
			return this->log_and_phony_response(
				e.code().message(), version, std::move(cache_ptr),
				server::http::status::internal_server_error
			);
	}
}


template<class HostType, class CacheType>
bool
server::file_host<HostType, CacheType>::parse_uri(const std::string &uri,
												  server::host_cache &cache)
{
	if (!server::host::parse_uri(uri, cache))
		return false;
	
	// Making the path relative
	cache.path = canonical("." / cache.path, this->file_host_parameters_.root);
	
	return true;
}
