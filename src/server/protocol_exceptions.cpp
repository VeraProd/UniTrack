// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol_exceptions.h>


server::protocol_error::protocol_error(const std::string &what_arg):
	std::logic_error(what_arg)
{}


// Start string
server::incorrect_start_string::incorrect_start_string(const std::string &str):
	server::protocol_error("Incorrect start string: \"" + str + "\"")
{}

server::unimplemented_method::unimplemented_method(const std::string &method_name):
	server::protocol_error("Unimplemented method: \"" + method_name + '\"')
{}

server::incorrect_protocol::incorrect_protocol(const std::string &protocol_name):
	server::protocol_error("Incorrect protocol: \"" + protocol_name + '\"')
{}

server::unsupported_protocol_version::unsupported_protocol_version(const std::string &version):
	server::protocol_error("Unsupported protocol version: \"" + version + '\"')
{}


// Headers
server::incorrect_header_string::incorrect_header_string(const std::string &str):
	server::protocol_error("Incorrect header string: \"" + str + '\"')
{}


server::empty_header_string::empty_header_string():
	server::protocol_error("Empty header string")
{}


server::header_required::header_required(const std::string &header_name):
	server::protocol_error("Header required: \"" + header_name + '\"')
{}


server::incorrect_host_header::incorrect_host_header(const std::string &host_str):
	server::protocol_error("Incorrect Host header: \"" + host_str + '\"')
{}


server::incorrect_port::incorrect_port(const std::string &port_str):
	server::protocol_error("Incorrect port: \"" + port_str + '\"')
{}
