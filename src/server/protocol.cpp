// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol.h>

#include <regex>
#include <cctype>

#include <server/protocol_exceptions.h>


// Removes trailing '\r' and '\n' symbols modifying the string
void
server::truncate_string(std::string &str) noexcept
{
	while (!str.empty()) {
		auto c = str.back();
		switch (c) {
			case '\n': case '\r':
				str.pop_back();
				break;
			default:
				return;
		}
	}
}


// Parses the given string and checks all details except uri
// Returns all data from start string or throws one of exceptions
// described in protocol_exceptions.h
server::start_data
server::parse_start_string(const std::string &str)
{
	static const std::regex
		regex("^"
			  "([_[:alnum:]]+)"		"[[:space:]]+"				// Method	[1]
			  "([^[:space:]]+)"		"[[:space:]]+"				// URI		[2]
			  "([_[:alnum:]]+)/([[:digit:]]+\\.[[:digit:]]+)"	// Protocol	[3]/[4]
			  "[\r\n]*$",
			  std::regex::optimize);
	
	static const auto str_toupper =
		[](std::string str) -> std::string
		{
			for (auto &ch: str) ch = std::toupper(ch);
			return str;
		};
	
	
	std::smatch match;	// Match results
	
	
	start_data data;
	if (!std::regex_match(str, match, regex) || match.size() != 5)
		throw server::incorrect_start_string(str);
	
	// Method validating
	{
		std::string method = str_toupper(match[1]);
		if 		(method == "GET"   ) data.method = server::http::method::GET;
		else if (method == "HEAD"  ) data.method = server::http::method::HEAD;
		else if (method == "POST"  ) data.method = server::http::method::POST;
		else if (method == "DELETE") data.method = server::http::method::DELETE;
		else throw server::unimplemented_method(method);
	}
	
	// Protocol validating
	{
		std::string protocol_name = str_toupper(match[3]);
		if (protocol_name != "HTTP") throw server::incorrect_protocol(protocol_name);
	}
	
	// Protocol version validating
	{
		std::string protocol_ver = str_toupper(match[4]);
		if (protocol_ver == "1.1") data.version = server::http::version::_1_1;
		else throw server::unsupported_protocol_version(protocol_ver);
	}
	
	// Setting up the uri
	data.uri = match[2];
	
	return data;
}


server::header_pair_t
server::parse_header_string(const std::string &str)
{
	static const std::regex
		regex("^"
			  "([^[:space:]]+)"		// Key [1]
			  ":[[:space:]]*"
			  "(.+)"				// Value [2]
			  "[\r\n]*$",
			  std::regex::optimize),
		regex_empty("^"
					"[\r[:space:]]*"
					"$",
					std::regex::optimize);
	
	std::smatch match;
	
	if (!std::regex_match(str, match, regex) || match.size() != 3) {
		if (std::regex_match(str, regex_empty))
			throw server::empty_header_string();
		throw server::incorrect_header_string(str);
	}
	
	return std::make_pair(match[1], match[2]);
}
