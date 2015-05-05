// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/module_registrar_exceptions.h>


templatizer::module_registrar_error::module_registrar_error(const std::string &what_arg):
	std::logic_error(what_arg)
{}


templatizer::module_not_found::module_not_found(const std::string &module_name):
	templatizer::module_registrar_error("Module \"" + module_name + "\" not found")
{}
