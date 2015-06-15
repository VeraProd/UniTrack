// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/modules/debug_chunk.h>

#include <templatizer/module.h>


const std::string templatizer::debug_chunk::cmd = "debug";


namespace {	// Module

templatizer::module<templatizer::debug_chunk> module(templatizer::debug_chunk::cmd);

};	// namespace


templatizer::debug_chunk::debug_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::debug_chunk::debug_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
void
templatizer::debug_chunk::generate(base::send_buffers_insert_iterator_t buffers_ins_it,
								   base::strings_cache_insert_functor_t cache_inserter,
								   const templatizer::model &model) const
{
	const auto &value = model.variable(this->symbol_);	// This can throw
	
	*buffers_ins_it = base::buffer(cache_inserter("DEBUG (length = "));
	*buffers_ins_it = base::buffer(cache_inserter(std::to_string(value.size())));
	*buffers_ins_it = base::buffer(cache_inserter("): \""));
	*buffers_ins_it = base::buffer(value);
	*buffers_ins_it = base::buffer(cache_inserter("\""));
}


void
templatizer::debug_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
