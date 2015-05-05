// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_MODULE_REGISTRAR_H
#define TEMPLATIZER_MODULE_REGISTRAR_H

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

#include <templatizer/chunk.h>


namespace templatizer {


typedef std::function<std::unique_ptr<templatizer::chunk> (const std::string &)> chunk_generator_t;


class module_registrar:
	private std::unordered_map<std::string, chunk_generator_t>
{
public:
	static module_registrar default_module_registrar;
	
	
	void add(const std::string &command, templatizer::chunk_generator_t &&generator);
	void erase(const std::string &command) noexcept;
	
	
	// Returns the chunk generator or throws templatizer::module_not_found exception.
	templatizer::chunk_generator_t at(const std::string &command) const;
};


};	// namespace templatizer


#endif // TEMPLATIZER_MODULE_REGISTRAR_H
