// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/modules/debug_chunk.h>

#include <boost/asio/buffer.hpp>

#include <templatizer/module.h>


const std::string templatizer::debug_chunk::debug_chunk_cmd = "debug";


namespace {	// Module

templatizer::module<templatizer::debug_chunk>
	test_chunk_module(templatizer::debug_chunk::debug_chunk_cmd);

};	// namespace


templatizer::debug_chunk::debug_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::debug_chunk::debug_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
void
templatizer::debug_chunk::generate(server::send_buffers_insert_iterator_t buffers_ins_it,
								   server::strings_cache_insert_iterator_t cache_ins_it,
								   server::strings_cache_extract_iterator_t cache_ext_it,
								   const templatizer::model &model) const
{
	const auto &value = model.variable(this->symbol_);	// This can throw
	
	*cache_ins_it = "DEBUG (length = ";
	*buffers_ins_it = boost::asio::buffer(*cache_ext_it);
	
	*cache_ins_it = std::to_string(value.size());
	*buffers_ins_it = boost::asio::buffer(*cache_ext_it);
	
	*cache_ins_it = "): \"";
	*buffers_ins_it = boost::asio::buffer(*cache_ext_it);
	
	*buffers_ins_it = boost::asio::buffer(value);
	
	*cache_ins_it = "\"";
	*buffers_ins_it = boost::asio::buffer(*cache_ext_it);
}


void
templatizer::debug_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
