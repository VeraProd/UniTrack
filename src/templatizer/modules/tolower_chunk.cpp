// Author: Vera Produvnova

#include <templatizer/modules/tolower_chunk.h>

#include <templatizer/module.h>



const std::string templatizer::tolower_chunk::cmd = "tolower";


namespace {	// Module

templatizer::module<templatizer::tolower_chunk> module(templatizer::tolower_chunk::cmd);

};	// namespace


templatizer::tolower_chunk::tolower_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::tolower_chunk::tolower_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
void
templatizer::tolower_chunk::generate(base::send_buffers_insert_iterator_t buffers_ins_it,
								 base::strings_cache_insert_functor_t cache_inserter,
								 const templatizer::model &model) const
{ 
	auto tolow = model.variable(this->symbol_);
	std::transform(tolow.begin(), tolow.end(), tolow.begin(), tolower);
	
	*buffers_ins_it = base::buffer(cache_inserter(tolow));
}


void
templatizer::tolower_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
