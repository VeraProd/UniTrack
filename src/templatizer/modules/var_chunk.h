// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_VAR_CHUNK_CPP
#define TEMPLATIZER_VAR_CHUNK_CPP

#include <string>

#include <templatizer/model.h>
#include <templatizer/module.h>


namespace templatizer {

class var_chunk: public templatizer::chunk
{
public:
	static const std::string var_chunk_cmd;	// = "var"
	
	var_chunk(const std::string &symbol) noexcept;
	var_chunk(std::string &&symbol) noexcept;
	
	virtual void generate(std::ostream &stream, const templatizer::model &model) const override;
	
	virtual void export_symbols(std::unordered_set<std::string> &symbols) const noexcept override;
private:
	const std::string symbol_;
};	// class var_chunk

};	// namespace templatizer

#endif // TEMPLATIZER_VAR_CHUNK_CPP
