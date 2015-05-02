// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/file_host.h>

#include <server/host_exceptions.h>


server::file_host::file_host(logger::logger &logger,
							 const file_host_parameters &parameters):
	server::host(logger, static_cast<const server::host_parameters &>(parameters)),
	
	file_host_parameters_(static_cast<const server::file_host_only_parameters &>(parameters))
{}


// virtual
server::host::send_buffers_t
server::file_host::response(const std::string &uri,
							server::http::method method,
							server::http::version version,
							const server::headers_t &request_headers,
							server::host::cache_t &strings_cache,
							const server::headers_t &response_headers)
{
	server::host::send_buffers_t res;
	
	return res;
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
