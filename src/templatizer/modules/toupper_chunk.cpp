// Author: Vera Produvnova

#include <templatizer/modules/toupper_chunk.h>

#include <templatizer/module.h>



const std::string templatizer::toupper_chunk::cmd = "toupper";


namespace {	// Module

templatizer::module<templatizer::toupper_chunk> module(templatizer::toupper_chunk::cmd);

};	// namespace


templatizer::toupper_chunk::toupper_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::toupper_chunk::toupper_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
void
templatizer::toupper_chunk::generate(base::send_buffers_insert_iterator_t buffers_ins_it,
								 base::strings_cache_insert_functor_t cache_inserter,
								 const templatizer::model &model) const
{ 
	auto toup = model.variable(this->symbol_);
	std::transform(toup.begin(), toup.end(), toup.begin(), toupper);
	
	*buffers_ins_it = base::buffer(cache_inserter(toup));
}


void
templatizer::toupper_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
