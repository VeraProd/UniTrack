// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <base/mapped_file_exceptions.h>


base::mapped_file_error::mapped_file_error(const std::string &what_arg):
	std::logic_error(what_arg)
{}


base::path_not_found::path_not_found(const std::string &path):
	base::mapped_file_error("Path not found: \"" + path + '\"')
{}


base::path_is_directory::path_is_directory(const std::string &path):
	base::mapped_file_error("Path is directory: \"" + path + '\"')
{}


base::incorrect_path::incorrect_path(const std::string &path):
	base::mapped_file_error("Path is incorrect: \"" + path + '\"')
{}
