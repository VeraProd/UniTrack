// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_OTHER_H
#define SERVER_OTHER_H

#include <string>


namespace server {
namespace other {


extern const std::string
	space_str,
	newline_str,
	HTTP_str,
	slash_str,
	header_separator_str,
	
	header_content_length,
	header_server,
	header_allow;


};	// namespace other
};	// namespace server

#endif // SERVER_OTHER_H
