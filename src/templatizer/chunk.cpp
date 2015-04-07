// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/chunk.h>


// class chunk
templatizer::chunk::~chunk() noexcept
{}


// class raw_chunk
templatizer::raw_chunk::raw_chunk(const char *data, size_t size) noexcept:
	data_(data),
	size_(size)
{}

void
templatizer::raw_chunk::generate(std::ostream &stream, const templatizer::model &model) const
{
	stream.write(this->data_, this->size_);
}

void
templatizer::raw_chunk::export_symbols(std::unordered_set<std::string> &) const noexcept
{}
