// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_PAGE_H
#define TEMPLATIZER_PAGE_H

#include <iostream>
#include <string>
#include <unordered_set>
#include <list>
#include <memory>
#include <type_traits>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <server/types.h>
#include <templatizer/chunk.h>
#include <templatizer/model.h>


namespace templatizer {


class page
{
public:
	enum class state {
		ok,
		file_error,
		parse_error
	};	// enum class state
	
	
	// Constructors
	page() noexcept;							// Default
	page(page &&other) noexcept;				// Move
	page(const std::string &file);				// Constructs and loads template data
	
	page & operator=(page &&other) noexcept;	// Move
	
	
	// Non-copy constructible/assignable
	page(const page &other) = delete;
	page & operator=(const page &other) = delete;
	
	
	// Loads template from file.
	// If an error occured, throws templatizer::file_mapping_error
	// or templatizer::file_parsing_error.
	void load(const std::string &file);
	
	// Simply deletes all loaded template data
	void clear() noexcept;
	
	
	// Generates result page from template using data model sending all data to the stream.
	// Returns number of sent bytes.
	size_t generate(std::ostream &stream,
					const templatizer::model &model) const;
	
	// Generates result page from template using data model adding all data to the buffers.
	// Returns summary size of all added buffers.
	size_t generate(server::send_buffers_t &buffers,
					const templatizer::model &model) const;
	
	
	// Syntax sugar, same as generate(). Usage:
	// 	std::cout << page(model);
	struct page_printer;
	inline page_printer operator()(const templatizer::model &model) const;
	
	
	// Symbols (variable names)
	typedef std::unordered_set<std::string> symbol_set;
	
	// All symbols need to get from model
	symbol_set symbols() const;
	
	// Same as symbols(), but puts them into set
	void export_symbols(symbol_set &symbols) const;
	
	
	// State
	inline enum state state() const noexcept;
	inline bool good() const noexcept;
	inline bool bad() const noexcept;
protected:
	inline void set_state(enum state new_state) noexcept;
	
	
	// Data
	typedef std::unique_ptr<templatizer::chunk> chunk_ptr_t;
	typedef std::list<chunk_ptr_t> chunk_ptrs_list_t;
private:
	// State
	enum state state_;
	
	
	// Data
	chunk_ptrs_list_t chunk_ptrs_;
	
	boost::interprocess::file_mapping file_mapping_;
	boost::interprocess::mapped_region mapped_region_;
};	// class page


};	// namespace templatizer


inline
std::ostream &
operator<<(std::ostream &stream, const templatizer::page::page_printer &printer);


#include <templatizer/page.hpp>

#endif // TEMPLATIZER_PAGE_H
