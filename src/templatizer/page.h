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
	page();									// Default
	page(const page &other) = delete;	// Do NOT copy!
	page(page &&other);				// Move
	page(const std::string &file);				// Constructs and loads template data
	
	
	// Loads template from file
	// Returns true, if all done ok (and *this contains new data).
	// Otherwise returns false (and *this still contains old data).
	bool load(const std::string &file);
	
	// Simply deletes all loaded template data
	void clear() noexcept;
	
	
	// Generates result page from template using data model
	std::string generate(const templatizer::model &model) const;
	
	// Same as generate()
	inline std::string operator()(const templatizer::model &model) const
	{ return this->generate(model); }
	
	
	// All symbols need to get from model
	std::unordered_set<std::string> symbols() const;
	
	
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
	
	
	// Data chunks
	class chunk
	{
	public:
		virtual ~chunk() noexcept = 0;
		
		virtual void init(const templatizer::model &model) const = 0;
		virtual void clear() const noexcept = 0;
		
		virtual const char * data() const = 0;
		virtual size_t size() const = 0;
		
		virtual std::string symbol() const noexcept = 0;
	};	// class chunk
	
	
	class raw_chunk: public chunk
	{
	public:
		raw_chunk(const char *data, size_t size) noexcept;
		
		virtual void init(const templatizer::model &model) const noexcept override;
		virtual void clear() const noexcept override;
		
		virtual const char * data() const noexcept override;
		virtual size_t size() const noexcept override;
		
		virtual std::string symbol() const noexcept override;
	private:
		char const * const data_;
		const size_t size_;
	};	// class raw_chunk
	
	
	class var_chunk: public chunk
	{
	public:
		var_chunk(const std::string &symbol) noexcept;
		var_chunk(std::string &&symbol) noexcept;
		
		virtual void init(const templatizer::model &model) const override;
		virtual void clear() const noexcept override;
		
		virtual const char * data() const noexcept override;
		virtual size_t size() const noexcept override;
		
		virtual std::string symbol() const noexcept override;
	private:
		const std::string symbol_;
		mutable std::string const *data_;
	};	// class var_chunk
	
	
	// Data
	typedef std::unique_ptr<chunk> chunk_ptr_t;
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

#endif // TEMPLATIZER_PAGE_H
