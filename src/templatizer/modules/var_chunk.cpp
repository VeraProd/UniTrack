// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/modules/var_chunk.h>


const std::string templatizer::var_chunk::var_chunk_cmd = "var";

namespace {	// Module
templatizer::module<templatizer::var_chunk> var_chunk_module(templatizer::var_chunk::var_chunk_cmd);
};


templatizer::var_chunk::var_chunk(const std::string &symbol) noexcept:
	symbol_(symbol)
{}

templatizer::var_chunk::var_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol))
{}


void
templatizer::var_chunk::generate(std::ostream &stream, const templatizer::model &model) const
{
	// This can throw:
	stream << model.at(this->symbol_);
}

void
templatizer::var_chunk::export_symbols(std::unordered_set<std::string> &symbols) const noexcept
{
	symbols.insert(this->symbol_);
}
