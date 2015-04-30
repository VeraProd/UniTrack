// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host_exceptions.h>


server::host_error::host_error(const std::string &what_arg):
	std::logic_error(what_arg)
{}


server::headers_has_content_length::headers_has_content_length(const std::string &value):
	server::host_error("Headers for phony page must NOT contain header "
					   "\"Content-Length\" (= " + value +')')
{}
