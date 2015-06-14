// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_VAR_CHUNK_CPP
#define TEMPLATIZER_VAR_CHUNK_CPP

#include <string>

#include <server/types.h>
#include <templatizer/chunk.h>
#include <templatizer/model.h>


namespace templatizer {


class var_chunk:
	public templatizer::chunk
{
public:
	static const std::string var_chunk_cmd;	// = "var"
	
	
	var_chunk(const std::string &symbol);
	var_chunk(std::string &&symbol) noexcept;
	
	
	virtual void generate(server::send_buffers_insert_iterator_t buffers_ins_it,
						  server::strings_cache_insert_iterator_t cache_ins_it,
						  server::strings_cache_extract_iterator_t cache_ext_it,
						  const templatizer::model &model) const override;
	
	
	virtual void export_symbols(std::unordered_set<std::string> &symbols) const override;
private:
	const std::string symbol_;
};	// class var_chunk


};	// namespace templatizer


#endif // TEMPLATIZER_VAR_CHUNK_CPP
