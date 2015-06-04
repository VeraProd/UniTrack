// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef BASE_MAPPED_FILE_EXCEPTIONS_H
#define BASE_MAPPED_FILE_EXCEPTIONS_H

#include <string>
#include <stdexcept>


namespace base {


class mapped_file_error: public std::logic_error
{
public:
	explicit mapped_file_error(const std::string &what_arg);
};


class path_not_found: public mapped_file_error
{
public:
	explicit path_not_found(const std::string &path);
};


class path_is_directory: public mapped_file_error
{
public:
	explicit path_is_directory(const std::string &path);
};


class incorrect_path: public mapped_file_error
{
public:
	explicit incorrect_path(const std::string &path);
};


};	// namespace base


#endif // BASE_MAPPED_FILE_EXCEPTIONS_H
