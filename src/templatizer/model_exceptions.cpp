// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/model_exceptions.h>


templatizer::model_error::model_error(const std::string &what_arg):
	std::logic_error(what_arg)
{}


templatizer::variable_not_found::variable_not_found(const std::string &var_name):
	templatizer::model_error("Variable not found: \"" + var_name + "\"")
{}
