// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/protocol_exceptions.h>


server::protocol_error::protocol_error(const std::string &what_arg):
	std::logic_error(what_arg)
{}

server::incorrect_start_string::incorrect_start_string(const std::string &str):
	protocol_error("Incorrect start string: \"" + str + "\"")
{}

server::unimplemented_method::unimplemented_method(const std::string &method_name):
	protocol_error("Unimplemented method: \"" + method_name + '\"')
{}

server::incorrect_protocol::incorrect_protocol(const std::string &protocol_name):
	protocol_error("Incorrect protocol: \"" + protocol_name + '\"')
{}

server::unsupported_protocol_version::unsupported_protocol_version(const std::string &version):
	protocol_error("Unsupported protocol version: \"" + version + '\"')
{}
