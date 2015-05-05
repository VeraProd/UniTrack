// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/chunk_exceptions.h>


templatizer::chunk_error::chunk_error(const std::string &what_arg):
	std::logic_error(what_arg)
{}
