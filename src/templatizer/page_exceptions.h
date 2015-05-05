// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_PAGE_EXCEPTIONS_H
#define TEMPLATIZER_PAGE_EXCEPTIONS_H

#include <string>
#include <stdexcept>


namespace templatizer {


class page_error: public std::logic_error
{
public:
	explicit page_error(const std::string &what_arg);
};


class file_mapping_error: public page_error
{
public:
	explicit file_mapping_error(const std::string &file_name, const std::string &message);
};


class file_parsing_error: public page_error
{
public:
	explicit file_parsing_error(const std::string &file_name, const std::string &message);
};


};	// namespace templatizer


#endif // TEMPLATIZER_PAGE_EXCEPTIONS_H
