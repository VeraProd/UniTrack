// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLETIZER_MODULE_H
#define TEMPLETIZER_MODULE_H

#include <string>

#include <templatizer/module_registrar.h>


namespace templatizer {

template<class ModuleRepr>
class module
{
public:
	module(const std::string &command,
		   templatizer::module_registrar &registrar = templatizer::default_module_registrar);
};

};	// namespace templatizer


#include <templatizer/module.hpp>

#endif // TEMPLETIZER_MODULE_H
