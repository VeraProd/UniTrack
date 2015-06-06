// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_PAGE_H
#define TEMPLATIZER_PAGE_H

#include <iostream>
#include <string>
#include <unordered_set>
#include <deque>
#include <memory>
#include <type_traits>

#include <boost/filesystem/path.hpp>

#include <base/mapped_file.h>

#include <server/types.h>
#include <templatizer/chunk.h>
#include <templatizer/model.h>


namespace templatizer {


class page:
	public base::mapped_file
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
	page(const boost::filesystem::path &path);	// Constructs and loads template data
	
	page & operator=(page &&other) noexcept;	// Move
	
	
	// Non-copy constructible/assignable
	page(const page &other) = delete;
	page & operator=(const page &other) = delete;
	
	
	// Loads template from file.
	// If an error occured, throws templatizer::file_mapping_error
	// or templatizer::file_parsing_error.
	void load(const boost::filesystem::path &path);
	
	// Loads template from current file.
	// If an error occured, throws templatizer::file_mapping_error
	// or templatizer::file_parsing_error.
	void load();
	
	
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
	inline enum state get_state() const noexcept;
	inline bool good() const noexcept;
	inline bool bad() const noexcept;
protected:
	inline void set_state(enum state new_state) noexcept;
	
	
	// Data
	typedef std::unique_ptr<templatizer::chunk> chunk_ptr_t;
	typedef std::deque<chunk_ptr_t> chunk_ptrs_deque_t;
private:
	// State
	state state_;
	
	
	// Data
	chunk_ptrs_deque_t chunk_ptrs_;
};	// class page


};	// namespace templatizer


inline
std::ostream &
operator<<(std::ostream &stream, const templatizer::page::page_printer &printer);


#include <templatizer/page.hpp>

#endif // TEMPLATIZER_PAGE_H
