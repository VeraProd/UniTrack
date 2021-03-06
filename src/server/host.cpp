// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host.h>

#include <chrono>
#include <mutex>
#include <regex>
#include <algorithm>

#include <server/host_exceptions.h>


namespace {


const std::string host_without_name = "";


};	// namespace


// static
std::unique_ptr<server::host> server::host::error_host_;


server::host::host(logger::logger &logger,
				   const server::host_parameters &parameters):
	logger::enable_logger(logger),
	
	host_parameters_(parameters),
	
	server_name_generator_(std::chrono::system_clock::now().time_since_epoch().count())
{}


// Returns true, if host can process requests on specified port, or false otherwise.
bool
server::host::port_allowed(server::port_t port) const noexcept
{
	if (this->host_parameters_.ports.find(port) == this->host_parameters_.ports.end())
		return false;
	return true;
}


// Returns host name as string (random!)
const std::string &
server::host::server_name() const noexcept
{
	if (this->host_parameters_.server_names.empty())
		return ::host_without_name;
	
	size_t index = this->server_name_generator_() % this->host_parameters_.server_names.size();
	return this->host_parameters_.server_names[index];
}


std::pair<const std::string *, bool>
server::host::server_name(const server::headers_t &response_headers,
						  const server::headers_t &additional_headers) const
{
	using namespace server::http;
	
	// Second indicates, is first the new generated server name or not
	std::pair<const std::string *, bool> res = { nullptr, true };
	
	auto it = response_headers.find(header_server);
	if (it != response_headers.end())
		res = { &it->second, false };
	
	it = additional_headers.find(header_server);
	if (it != additional_headers.end()) {
		if (res.second) res = { &it->second, false };
		else throw server::duplicate_header(header_server);
	}
	
	if (res.second) res.first = &this->server_name();
	
	return res;
}


// Prepares a correct response to the client. By default -- phony "404 Not Found".
// Returns pair<vector<buffer>, shared_ptr<cache>> ready to socket.async_send().
// WARNING: first field of result does NOT contain data, only references. Second field
// contains data need to be sent, so save the given shared_ptr anywhere during all sending!
// virtual
server::response_data_t
server::host::response(std::string && /*uri*/,							// Unused
					   server::http::method /*method*/,					// Unused
					   server::http::version version,
					   server::headers_t && /*request_headers*/,		// Unused
					   server::headers_t &&response_headers)
{
	return this->phony_response(version,
								server::http::status::not_found,
								std::move(response_headers));
}


// Prepares a phony response to the client.
// Returns vector<buffer> ready to socket.async_send().
// WARNING: see notes to the response() method, remember to save anywhere status too
// (standard statuses are already saved)! response_headers must NOT contain "Content-Length"!
server::response_data_t
server::host::phony_response(server::http::version version,
							 const server::http::status &status,
							 server::headers_t &&response_headers,
							 server::headers_t &&additional_headers)
{
	using namespace server::http;
	using base::buffer;
	
	
	// Headers must NOT contain "Content-Length"!
	server::host::validate_headers(response_headers);
	server::host::validate_headers(additional_headers);
	
	
	auto cache_ptr = std::make_shared<server::host_cache>();
	cache_ptr->response_headers = std::move(response_headers);
	cache_ptr->additional_headers = std::move(additional_headers);
	
	
	auto server_name = this->server_name(cache_ptr->response_headers, cache_ptr->additional_headers);
	
	
	base::send_buffers_t res;
	res.reserve(16 + 4 * (cache_ptr->response_headers.size()
						  + cache_ptr->additional_headers.size()
						  + ((server_name.second)? 1: 0)));
	
	
	// Start string
	server::host::add_start_string(res, version, status);
	
	
	// Headers
	if (server_name.second)
		server::host::add_header(res, header_server, *server_name.first);
	server::host::add_headers(res, cache_ptr->response_headers);
	server::host::add_headers(res, cache_ptr->additional_headers);
	
	
	// Inserting Content-Length and page content
	{
		auto
			content_prefix_it = cache_ptr->strings.emplace(
				cache_ptr->strings.end(),
				"<html>"
				"<head>"
					"<meta charset=\"utf-8\">"
					"<title>" + status.code_str() + space_str + status.description() + "</title>"
				"</head>"
				"<body>"
					"<h1>" + status.code_str() + space_str + status.description() + "</h1>"
					"<hr width=\"100%\">"
					"<p>"
			),
			
			content_suffix_it = cache_ptr->strings.emplace(
				cache_ptr->strings.end(),
					"</p>"
				"</body>"
				"</html>"
			),
			
			len_it = cache_ptr->strings.emplace(
				cache_ptr->strings.end(),
				std::to_string(content_prefix_it->size()
							   + server_name.first->size()
							   + content_suffix_it->size())
			);
		
		server::host::add_header(res, header_content_length, *len_it);
		server::host::finish_headers(res);
		
		// Content
		server::host::add_buffer(res, buffer(*content_prefix_it));
		server::host::add_buffer(res, buffer(*server_name.first));
		server::host::add_buffer(res, buffer(*content_suffix_it));
	}
	
	return { std::move(res), std::move(cache_ptr) };
}


// Returns reference to error host object, creating it, if does not exist.
// If it didn't exist, it will be binded to logger of object's, whoose method was called.
server::host &
server::host::error_host() const
{
	return server::host::error_host(this->logger());
}


// Returns reference to error host object, creating it, if does not exist.
// If it didn't exist, it will be binded to logger.
// static
server::host &
server::host::error_host(logger::logger &logger)
{
	if (server::host::error_host_ == nullptr)
		server::host::create_error_host(logger);
	return *server::host::error_host_;
}


// Creates error_host if it does not exist. You may call it once from server, if you want.
// static
void
server::host::create_error_host(logger::logger &logger)
{
	static std::mutex m;
	
	std::unique_lock<std::mutex> lock(m);
	if (server::host::error_host_ == nullptr)
		server::host::error_host_ = std::make_unique<server::host>(logger,
																   server::host_parameters());
}


// Response forming helpers
// static
void
server::host::add_start_string(base::send_buffers_t &buffers,
							   server::http::version version,
							   const server::http::status &status)
{
	using base::buffer;
	using namespace server::http;
	
	buffers.insert(
		buffers.end(),
		{
			buffer(HTTP_str),
			buffer(slash_str),
			buffer(server::http::version_to_str(version)),
			buffer(space_str),
			buffer(status.code_str()),
			buffer(space_str),
			buffer(status.description()),
			buffer(newline_str)
		}
	);
}


// static
void
server::host::add_header(base::send_buffers_t &buffers,
						 const std::string &header,
						 const std::string &value)
{
	using base::buffer;
	using namespace server::http;
	
	buffers.insert(
		buffers.end(),
		{
			buffer(header),
			buffer(header_separator_str),
			buffer(value),
			buffer(newline_str)
		}
	);
}


// static
void
server::host::add_header(base::send_buffers_t &buffers,
						 const server::header_pair_t &header)
{
	using base::buffer;
	using namespace server::http;
	
	buffers.insert(
		buffers.end(),
		{
			buffer(header.first),
			buffer(header_separator_str),
			buffer(header.second),
			buffer(newline_str)
		}
	);
}


// static
void
server::host::add_headers(base::send_buffers_t &buffers,
						  const server::headers_t &headers)
{
	buffers.reserve(buffers.size() + 4 * headers.size());
	
	for (const auto &header: headers)
		server::host::add_header(buffers, header);
}


// static
void
server::host::finish_headers(base::send_buffers_t &buffers)
{
	using base::buffer;
	using namespace server::http;
	
	buffers.push_back(buffer(newline_str));
}


// static
void
server::host::add_buffer(base::send_buffers_t &buffers,
						 const base::send_buffer_t &buffer)
{
	buffers.push_back(buffer);
}


// static
void
server::host::validate_headers(const server::headers_t &headers)
{
	using namespace server::http;
	
	if (headers.find(header_content_length) != headers.end())
		throw server::headers_has_content_length();
}


// static
bool
server::host::parse_uri(const std::string &uri, server::host_cache &cache)
{
	static const std::regex
		base_regex(
			"([^?]+)(\\?(.*))?",				// [1], [4]: path and args
			std::regex::optimize
		),
		
		args_regex(
			"(\\&|\\?)?"						// [1]: '&' or '?'
			"("
				"([^\\&\\?]+)\\=([^\\&\\?]*)"	// [3], [4]: key, value
			"|"
				"([^\\&\\?]+)"					// [5]: key only
			")",
			std::regex::optimize
		);
	
	static const std::regex_iterator<std::string::const_iterator> end;
	
	
	std::smatch m;
	if (std::regex_match(uri, m, base_regex)) {
		cache.path = m[1].str();
		
		auto args_sm = m[3];
		if (args_sm.length() != 0) {
			std::string args = args_sm;
			
			auto begin = std::regex_iterator<std::string::const_iterator>(args.begin(), args.end(),
																		  args_regex);
			std::for_each(begin, end,
				[&cache] (const auto &m) {
					auto key = m[3];
					if (key.length() == 0) {
						cache.args_set.emplace(m[5]);		// Key without value
					} else {
						cache.args_map.emplace(key, m[4]);	// Key-Value pair
					}
				});
		}
	} else {
		return false;
	}
	
	return true;
}
