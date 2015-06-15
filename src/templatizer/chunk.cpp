// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/chunk.h>


// class raw_chunk
templatizer::raw_chunk::raw_chunk(const char *data, size_t size) noexcept:
	data_(data),
	size_(size)
{}


// virtual
void
templatizer::raw_chunk::generate(base::send_buffers_insert_iterator_t buffers_ins_it,
								 base::strings_cache_insert_functor_t /*cache_inserter*/,
								 const templatizer::model &model) const
{
	*buffers_ins_it = base::buffer(this->data_, this->size_);
}


// virtual
void
templatizer::raw_chunk::export_symbols(std::unordered_set<std::string> &) const noexcept
{}
