// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_MODEL_EXCEPTIONS_H
#define TEMPLATIZER_MODEL_EXCEPTIONS_H

#include <string>
#include <stdexcept>


namespace templatizer {


class model_error: public std::logic_error
{
public:
	explicit model_error(const std::string &what_arg);
};


class variable_not_found: public model_error
{
public:
	explicit variable_not_found(const std::string &var_name);
};


};	// namespace templatizer


#endif // TEMPLATIZER_MODEL_EXCEPTIONS_H
