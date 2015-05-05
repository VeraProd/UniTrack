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
size_t
templatizer::var_chunk::generate(std::ostream &stream,
								 const templatizer::model &model) const
{
	// This can throw:
	stream << model.variable(this->symbol_);
	return this->symbol_.size();
}


// virtual
size_t
templatizer::var_chunk::generate(server::send_buffers_t &buffers,
								 const templatizer::model &model) const
{
	buffers.emplace_back(boost::asio::buffer(model.variable(this->symbol_)));
	return this->symbol_.size();
}


void
templatizer::var_chunk::export_symbols(std::unordered_set<std::string> &symbols) const
{
	symbols.insert(this->symbol_);
}
