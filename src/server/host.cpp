// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host.h>

#include <chrono>
#include <mutex>

#include <server/host_exceptions.h>


namespace {


const std::string host_without_name = "";


};	// namespace


// static
std::unique_ptr<server::host> server::host::error_host_;


server::host::host(logger::logger &logger,
				   const server::host_parameters &parameters):
	host_parameters_(parameters),
	
	logger_(logger),
	
	server_name_generator_(std::chrono::system_clock::now().time_since_epoch().count())
{}


server::host::~host()
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


// Prepares a correct response to the client. By default -- phony "404 Not Found".
// Returns vector<buffer> ready to socket.async_send().
// WARNING: result of this function does NOT contain the data, only references,
// so rebember to save the data anywhere. uri, request_headers, strings_cache
// and response_headers must be correct during all socket.async_send()!
// strings_cache will contain some cached data.
// virtual
server::host::send_buffers_t
server::host::response(const std::string & /*uri*/,						// Unused
					   server::http::method /*method*/,					// Unused
					   server::http::version version,
					   const server::headers_t & /*request_headers*/,	// Unused
					   server::host::cache_t &strings_cache,
					   const server::headers_t &response_headers)
{
	return std::move(this->phony_response(version,
										  server::http::status::not_found,
										  strings_cache,
										  response_headers));
}


// Prepares a phony response to the client.
// Returns vector<buffer> ready to socket.async_send().
// WARNING: see notes to the response() method, remember to save anywhere status too!
// response_headers must NOT contain "Content-Length"!
server::host::send_buffers_t
server::host::phony_response(server::http::version version,
							 const server::http::status &status,
							 server::host::cache_t &strings_cache,
							 const server::headers_t &response_headers)
{
	using namespace server::http;
	using boost::asio::buffer;
	
	
	const std::string *current_server_name = nullptr;
	bool use_new_server_name = true;
	
	// response_Headers must NOT contain "Content-Length"!
	{
		auto it = response_headers.find(header_content_length);
		if (it != response_headers.end())
			throw server::headers_has_content_length(it->second);
		
		it = response_headers.find(header_server);
		if (it == response_headers.end()) {
			current_server_name = &this->server_name();
			use_new_server_name = true;
		} else {
			current_server_name = &it->second;
			use_new_server_name = false;
		}
	}
	
	
	server::host::send_buffers_t res;
	res.reserve(15 + 4 * (response_headers.size() + ((use_new_server_name)? 1: 0)));
	
	
	// Start string
	server::host::add_start_string(res, version, status);
	
	
	// Headers
	if (use_new_server_name)
		server::host::add_header(res, header_server, *current_server_name);
	server::host::add_headers(res, response_headers);
	
	
	// Inserting Content-Length and page content
	{
		auto
			content_prefix_it = strings_cache.emplace(strings_cache.end(),
				"<html>"
				"<head>"
					"<meta charset=\"utf-8\">"
					"<title>" + status.code_str() + space_str + status.description() + "</title>"
				"</head>"
				"<body>"
					"<h1>" + status.code_str() + space_str + status.description() + "</h1>"
					"<hr width=\"100%\">"
					"<p>"),
			
			content_suffix_it = strings_cache.emplace(strings_cache.end(),
					"</p>"
				"</body>"
				"</html>"
			),
			
			len_it = strings_cache.emplace(strings_cache.end(),
				std::to_string(content_prefix_it->size()
							   + current_server_name->size()
							   + content_suffix_it->size()));
		
		server::host::add_header(res, header_content_length, *len_it);
		server::host::finish_headers(res);
		
		// Content
		server::host::add_buffer(res, buffer(*content_prefix_it));
		server::host::add_buffer(res, buffer(*current_server_name));
		server::host::add_buffer(res, buffer(*content_suffix_it));
	}
	
	return res;
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
		server::host::error_host_.reset(new server::host(logger, server::host_parameters()));
}


// Response forming helpers
// static
void
server::host::add_start_string(send_buffers_t &buffers,
							   server::http::version version,
							   const server::http::status &status)
{
	using boost::asio::buffer;
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
		});
}


// static
void
server::host::add_header(send_buffers_t &buffers,
						 const std::string &header,
						 const std::string &value)
{
	using boost::asio::buffer;
	using namespace server::http;
	
	buffers.insert(
		buffers.end(),
		{
			buffer(header),
			buffer(header_separator_str),
			buffer(value),
			buffer(newline_str)
		});
}


// static
void
server::host::add_header(send_buffers_t &buffers,
						 const server::header_pair_t &header)
{
	using boost::asio::buffer;
	using namespace server::http;
	
	buffers.insert(
		buffers.end(),
		{
			buffer(header.first),
			buffer(header_separator_str),
			buffer(header.second),
			buffer(newline_str)
		});
}


// static
void
server::host::add_headers(send_buffers_t &buffers,
						  const server::headers_t &headers)
{
	buffers.reserve(buffers.size() + 4 * headers.size());
	
	for (const auto &header: headers)
		server::host::add_header(buffers, header);
}


// static
void
server::host::finish_headers(send_buffers_t &buffers)
{
	using boost::asio::buffer;
	using namespace server::http;
	
	buffers.push_back(buffer(newline_str));
}


// static
void
server::host::add_buffer(send_buffers_t &buffers,
						 const send_buffer_t &buffer)
{
	buffers.push_back(buffer);
}
