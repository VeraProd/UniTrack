// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/modules/var_chunk.h>

#include <boost/asio/buffer.hpp>

#include <templatizer/module.h>


const std::string templatizer::var_chunk::var_chunk_cmd = "var";


namespace {	// Module

templatizer::module<templatizer::var_chunk> var_chunk_module(templatizer::var_chunk::var_chunk_cmd);

};	// namespace


templatizer::var_chunk::var_chunk(const std::string &symbol):
	symbol_(symbol)
{}


templatizer::var_chunk::var_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


// virtual
void
templatizer::var_chunk::generate(server::send_buffers_insert_iterator_t buffers_ins_it,
								 server::strings_cache_insert_iterator_t /*cache_ins_it*/,
								 server::strings_cache_extract_iterator_t /*cache_ext_it*/,
								 const templatizer::model &model) const
{
	*buffers_ins_it = boost::asio::buffer(model.variable(this->symbol_));
}


void
templatizer::var_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
