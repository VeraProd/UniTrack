// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_CHUNK_H
#define TEMPLATIZER_CHUNK_H

#include <iostream>
#include <string>
#include <unordered_set>

#include <server/types.h>
#include <templatizer/model.h>


namespace templatizer {


// Data chunks
class chunk
{
public:
	virtual ~chunk() = 0;
	
	
	// Generates page content sending it to the stream.
	// Returns number of sent bytes.
	virtual size_t generate(std::ostream &stream,
							const templatizer::model &model) const = 0;
	
	
	// Generates page content adding it to the buffers.
	// Returns size of added buffer.
	virtual size_t generate(server::send_buffers_t &buffers,
							const templatizer::model &model) const = 0;
	
	
	virtual void export_symbols(std::unordered_set<std::string> &symbols) const = 0;
};	// class chunk


class raw_chunk: public chunk
{
public:
	raw_chunk(const char *data, size_t size) noexcept;
	
	
	virtual size_t generate(std::ostream &stream,
							const templatizer::model &model) const override;
	
	
	virtual size_t generate(server::send_buffers_t &buffers,
							const templatizer::model &model) const override;
	
	
	virtual void export_symbols(std::unordered_set<std::string> &symbols) const noexcept override;
private:
	char const * const data_;
	const size_t size_;
};	// class raw_chunk


};	// namespace templatizer


#endif // TEMPLATIZER_CHUNK_H
