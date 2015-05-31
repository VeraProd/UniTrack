// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_MODULE_H
#define TEMPLATIZER_MODULE_H

#include <string>

#include <templatizer/module_registrar.h>


namespace templatizer {


template<class ModuleRepr>
class module
{
public:
	module(
		const std::string &command,
		templatizer::module_registrar &registrar =
			templatizer::module_registrar::default_module_registrar
	);
};	// class module


};	// namespace templatizer


#include <templatizer/module.hpp>

#endif // TEMPLATIZER_MODULE_H
