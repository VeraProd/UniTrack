// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <memory>


template<class ModuleRepr>
templatizer::module<ModuleRepr>::module(const std::string &command,
										templatizer::module_registrar &registrar)
{
	registrar.add(
		command,
		[](const std::string &argument) -> std::unique_ptr<templatizer::chunk> {
			return std::make_unique<ModuleRepr>(argument);
		}
	);
}
