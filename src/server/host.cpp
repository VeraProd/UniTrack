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
				   const host_parameters &parameters):
	logger_(logger),
	parameters_(parameters),
	
	server_name_generator_(std::chrono::system_clock::now().time_since_epoch().count())
{}


// Returns host name as string (random!)
const std::string &
server::host::server_name() const noexcept
{
	if (this->parameters_.server_names.empty())
		return ::host_without_name;
	
	size_t index = this->server_name_generator_() % this->parameters_.server_names.size();
	return this->parameters_.server_names[index];
}


// Returns vector<buffer> ready to socket.async_send()
// WARNING: result of this function does NOT contain the data, only references,
// so rebember to save the data anywhere. status and headers must be correct
// during all socket.async_send()! strings_cache contains some cached data.
// Caller must store it as long as it need too.
std::vector<boost::asio::const_buffer>
server::host::phony_page(server::http::version version,
						 const server::http::status &status,
						 server::host::cache_t &strings_cache,
						 const server::headers_t &headers)
{
	using namespace server::http;
	using boost::asio::const_buffer;
	using boost::asio::buffer;
	
	
	const std::string *current_server_name = nullptr;
	bool use_new_server_name = true;
	
	// Headers must NOT contain "Content-Length"!
	{
		auto it = headers.find(header_content_length);
		if (it != headers.end())
			throw server::headers_has_content_length(it->second);
		
		it = headers.find(header_server);
		if (it == headers.end()) {
			current_server_name = &this->server_name();
			use_new_server_name = true;
		} else {
			current_server_name = &it->second;
			use_new_server_name = false;
		}
	}
	
	
	std::vector<boost::asio::const_buffer> res;
	res.reserve(16 + 4 * (headers.size() + ((use_new_server_name)? 1: 0)));
	
	
	// Start string
	res.emplace_back(buffer(HTTP_str));
	res.emplace_back(buffer(slash_str));
	res.emplace_back(buffer(server::http::version_to_str(version)));
	res.emplace_back(buffer(space_str));
	res.emplace_back(buffer(status.code_str()));
	res.emplace_back(buffer(space_str));
	res.emplace_back(buffer(status.description()));
	res.emplace_back(buffer(newline_str));
	
	
	// Headers
	if (use_new_server_name) {
		res.emplace_back(buffer(header_server));
		res.emplace_back(buffer(header_separator_str));
		res.emplace_back(buffer(*current_server_name));
		res.emplace_back(buffer(newline_str));
	}
	
	for (const auto &header: headers) {
		res.emplace_back(buffer(header.first));
		res.emplace_back(buffer(header_separator_str));
		res.emplace_back(buffer(header.second));
		res.emplace_back(buffer(newline_str));
	}
	
	
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
		
		res.emplace_back(buffer(header_content_length));
		res.emplace_back(buffer(header_separator_str));
		res.emplace_back(buffer(*len_it));
		res.emplace_back(buffer(newline_str));
		res.emplace_back(buffer(newline_str));
		
		res.emplace_back(buffer(*content_prefix_it));
		res.emplace_back(buffer(*current_server_name));
		res.emplace_back(buffer(*content_suffix_it));
	}
	
	return res;
}


// Returns reference to error host object, creating it, if does not exist.
// If it didn't exist, it will be binded to logger of object's, whoose method was called.
server::host &
server::host::error_host() const
{
	return server::host::error_host(this->logger_);
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
void
server::host::create_error_host(logger::logger &logger)
{
	static std::mutex m;
	
	std::unique_lock<std::mutex> lock(m);
	if (server::host::error_host_ == nullptr)
		server::host::error_host_.reset(new server::host(logger, server::host_parameters()));
}
