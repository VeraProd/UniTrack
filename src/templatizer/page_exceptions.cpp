// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/page_exceptions.h>


templatizer::page_error::page_error(const std::string &what_arg):
	std::logic_error(what_arg)
{}


templatizer::file_mapping_error::file_mapping_error(const std::string &file_name,
													const std::string &message):
	templatizer::page_error("File \"" + file_name + "\" mapping error: " + message)
{}


templatizer::file_parsing_error::file_parsing_error(const std::string &file_name,
													const std::string &message):
	templatizer::page_error("File \"" + file_name + "\" parsing error: " + message)
{}
