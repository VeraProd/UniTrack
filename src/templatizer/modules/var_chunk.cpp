// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/modules/var_chunk.h>

#include <templatizer/module.h>


const std::string templatizer::var_chunk::cmd = "var";


namespace {	// Module

templatizer::module<templatizer::var_chunk> module(templatizer::var_chunk::cmd);

};	// namespace


templatizer::var_chunk::var_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::var_chunk::var_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
void
templatizer::var_chunk::generate(base::send_buffers_insert_iterator_t buffers_ins_it,
								 base::strings_cache_insert_functor_t /*cache_inserter*/,
								 const templatizer::model &model) const
{
	*buffers_ins_it = base::buffer(model.variable(this->symbol_));
}


void
templatizer::var_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
