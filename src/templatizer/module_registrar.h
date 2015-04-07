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
	module_registrar();
	~module_registrar();
	
	void add(const std::string &command, templatizer::chunk_generator_t &&generator) noexcept;
	void erase(const std::string &command) noexcept;
	
	templatizer::chunk_generator_t at(const std::string &command) const;
};


extern module_registrar default_module_registrar;

};	// namespace templatizer

#endif // TEMPLATIZER_MODULE_REGISTRAR_H
