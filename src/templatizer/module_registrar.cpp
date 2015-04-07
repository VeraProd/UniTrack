// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/module_registrar.h>


templatizer::module_registrar templatizer::default_module_registrar;


templatizer::module_registrar::module_registrar()
{}

templatizer::module_registrar::~module_registrar()
{}


void
templatizer::module_registrar::add(const std::string &command, templatizer::chunk_generator_t &&generator) noexcept
{
	this->emplace(command, std::move(generator));
}

void
templatizer::module_registrar::erase(const std::string &command) noexcept
{
	this->erase(command);
}


templatizer::chunk_generator_t
templatizer::module_registrar::at(const std::string &command) const
{
	auto it = this->find(command);
	if (it == this->end())
		throw std::out_of_range("Command \"" + command + "\" not found.");
	return it->second;
}
