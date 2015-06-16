// Author: Vera Produvnova

#include <templatizer/modules/date_chunk.h>

#include <templatizer/module.h>

#include <ctime>


const std::string templatizer::date_chunk::cmd = "date";


namespace {	// Module

templatizer::module<templatizer::date_chunk> module(templatizer::date_chunk::cmd);

};	// namespace

templatizer::date_chunk::date_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::date_chunk::date_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
void
templatizer::date_chunk::generate(base::send_buffers_insert_iterator_t buffers_ins_it,
								  base::strings_cache_insert_functor_t cache_inserter,
								  const templatizer::model &model) const
{
	time_t raw_time;
	::time(&raw_time);
	auto *time_info = ::localtime(&raw_time);
	char date_str[11];
	::strftime(date_str, 11, "%d.%m.%Y", time_info);
	
	*buffers_ins_it = base::buffer(cache_inserter(date_str));
	
}

void
templatizer::date_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
