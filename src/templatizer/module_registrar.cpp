// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/module_registrar.h>

#include <templatizer/module_registrar_exceptions.h>


// static
templatizer::module_registrar templatizer::module_registrar::default_module_registrar;


void
templatizer::module_registrar::add(const std::string &command,
								   templatizer::chunk_generator_t &&generator)
{
	this->emplace(command, std::move(generator));
}


void
templatizer::module_registrar::erase(const std::string &command) noexcept
{
	this->erase(command);
}


templatizer::chunk_generator_t
templatizer::module_registrar::module(const std::string &command) const
{
	auto it = this->find(command);
	if (it == this->end())
		throw templatizer::module_not_found(command);
	return it->second;
}
