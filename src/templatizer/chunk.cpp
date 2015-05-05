// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/chunk.h>

#include <boost/asio/buffer.hpp>


// class chunk
templatizer::chunk::~chunk()
{}


// class raw_chunk
templatizer::raw_chunk::raw_chunk(const char *data, size_t size) noexcept:
	data_(data),
	size_(size)
{}


// virtual
size_t
templatizer::raw_chunk::generate(std::ostream &stream,
								 const templatizer::model &model) const
{
	stream.write(this->data_, this->size_);
	return this->size_;
}


// virtual
size_t
templatizer::raw_chunk::generate(server::send_buffers_t &buffers,
								 const templatizer::model &model) const
{
	buffers.emplace_back(boost::asio::buffer(this->data_, this->size_));
	return this->size_;
}


// virtual
void
templatizer::raw_chunk::export_symbols(std::unordered_set<std::string> &) const noexcept
{}
