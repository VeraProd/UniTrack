// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_PAGE_H
#define TEMPLATIZER_PAGE_H

#include <iostream>
#include <string>
#include <unordered_set>
#include <list>
#include <memory>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <templatizer/chunk.h>
#include <templatizer/model.h>


namespace templatizer {

// State
namespace page_state {
	extern const int ok,			// = 0
					 file_error,	// = 10
					 parse_error;	// = 20
};


class page
{
public:
	// Constructors
	page();								// Default
	page(const page &other) = delete;	// Do NOT copy!
	page(page &&other);					// Move
	page(const std::string &file);		// Constructs and loads template data
	
	
	// Loads template from file
	// Returns true, if all done ok (and *this contains new data).
	// Otherwise returns false (and *this still contains old data).
	bool load(const std::string &file);
	
	// Simply deletes all loaded template data
	void clear() noexcept;
	
	
	// Generates result page from template using data model
	void generate(std::ostream &stream, const templatizer::model &model) const;
	
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
	inline int state() const
	{ return this->state_; }
	
	inline bool good() const
	{ return this->state_ == page_state::ok; }
	
	inline bool bad() const
	{ return this->good(); }
protected:
	// State
	void set_state(int new_state);
	
	
	// Data
	typedef std::unique_ptr<templatizer::chunk> chunk_ptr_t;
	typedef std::list<chunk_ptr_t> chunk_list_t;
private:
	// State
	int state_;
	
	
	// Data
	chunk_list_t chunks_;
	
	boost::interprocess::file_mapping mapping_;
	boost::interprocess::mapped_region region_;
};	// class page

};	// namespace templatizer


inline
std::ostream &
operator<<(std::ostream &stream, const templatizer::page::page_printer &printer);


#include <templatizer/page.hpp>

#endif // TEMPLATIZER_PAGE_H
