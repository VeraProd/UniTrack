// Author: Vera Produvnova

#include <templatizer/modules/time_chunk.h>

#include <templatizer/module.h>

#include <ctime>


const std::string templatizer::time_chunk::cmd = "time";


namespace {	// Module

templatizer::module<templatizer::time_chunk> module(templatizer::time_chunk::cmd);

};	// namespace


templatizer::time_chunk::time_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::time_chunk::time_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
void
templatizer::time_chunk::generate(base::send_buffers_insert_iterator_t buffers_ins_it,
								  base::strings_cache_insert_functor_t cache_inserter,
								  const templatizer::model &model) const
{
	time_t raw_time;
	::time(&raw_time);
	auto *time_info = ::localtime(&raw_time);
	char time_str[6];
	::strftime(time_str, 6, "%H:%M", time_info);
	
	*buffers_ins_it = base::buffer(cache_inserter(time_str));
	
}


void
templatizer::time_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
