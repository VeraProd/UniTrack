// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/page.h>

#include <fstream>
#include <regex>


// State
const int templatizer::page_state::ok			=  0,
		  templatizer::page_state::file_error	= 10,
		  templatizer::page_state::parse_error	= 20;


// Default constructor
templatizer::page::page():
	state_(templatizer::page_state::ok)
{}

// Move constructor
templatizer::page::page(templatizer::page &&other):
	state_(std::move(other.state_)),
	chunks_(std::move(other.chunks_)),
	mapping_(std::move(other.mapping_)),
	region_(std::move(other.region_))
{}

// Constructs and loads template data
templatizer::page::page(const std::string &file):
	state_(templatizer::page_state::ok)
{ this->load(file); }


// Loads template from file
bool
templatizer::page::load(const std::string &file)
{
	using namespace boost::interprocess;
	
	try {
		file_mapping mapping(file.c_str(), read_only);
		mapped_region region(mapping, read_only);
		chunk_list_t chunks;
		
		// Parsing
		{
			const char *region_data = static_cast<const char *>(region.get_address());
			size_t region_size = region.get_size() / sizeof(char);
			
			std::cerr << "Here!" << std::endl
					  << "Read characters: " << region_size << std::endl
					  << region_data << std::endl;
			
			// REGEX: $(VAR_NAME)
			// VAR_NAME can contain characters: a-z, A-Z, 0-9 and '_', but must to begin with
			// letter (like in C). Minimum length of name is 1 symbol.
			// NOTE: Some space symbols around VAR_NAME is allowed (but not a newline).
			// VAR_NAME is a first group of the regex.
			std::regex regex("\\$([[:blank:]]*\\([[:alpha:]_][[:alnum:]_]*\\)[[:blank:]]*)",
							 std::regex::grep);
			
			std::regex_iterator<const char *>
				it(region_data, region_data + region_size, regex),
				end;
			
			size_t old_pos = 0;
			while (it != end) {
				size_t current_pos = it->position();
				if (current_pos > old_pos) {	// Indexing previous raw chunk...
					chunks.emplace_back(new raw_chunk(region_data + old_pos, current_pos - old_pos));
					old_pos = current_pos + it->length();
				}
				chunks.emplace_back(new var_chunk(std::move(it->str(1))));	// ...and founded var chunk
				
				++it;
			}
			
			// Remember to index the last raw chunk
			if (old_pos < region_size)
				chunks.emplace_back(new raw_chunk(region_data + old_pos, region_size - old_pos));
		}	// End of parsing
		
		// Move data to *this, if success (if not success, see catch blocks below)
		this->chunks_	= std::move(chunks);
		this->mapping_	= std::move(mapping);
		this->region_	= std::move(region);
		
		this->set_state(templatizer::page_state::ok);
	} catch (interprocess_exception &e) {
		std::cerr << "Can't map file: \"" << file << "\": " << e.what() << std::endl;
		this->set_state(templatizer::page_state::file_error);
	} catch (...) {
		std::cerr << "Can't parse file: \"" << file << "\": " << std::endl;
		this->set_state(templatizer::page_state::parse_error);
	}
	
	return this->good();
}

// Simply deletes all loaded template data
void
templatizer::page::clear() noexcept
{
	using namespace boost::interprocess;
	
	this->chunks_.clear();
	this->mapping_ = std::move(file_mapping());
	this->region_ = std::move(mapped_region());
	this->set_state(templatizer::page_state::ok);
}


// Generates result page from template using data model
std::string
templatizer::page::generate(const templatizer::model &model) const
{
	std::string res;
	for (const auto &chunk: this->chunks_) {
		chunk->init(model);	// This can throw (using model.at())!
		
		size_t chunk_size = chunk->size();
		if (chunk_size > 0)
			res.append(chunk->data(), chunk_size);
		
		chunk->clear();
	}
	return res;
}


// All symbols need to get from model
std::unordered_set<std::string>
templatizer::page::symbols() const
{
	std::unordered_set<std::string> res;
	for (const auto &chunk: this->chunks_) {
		auto &&sym = chunk->symbol();
		if (!sym.empty()) res.insert(sym);
	}
	return res;
}


// State
void
templatizer::page::set_state(int new_state)
{
	switch (new_state) {
		case templatizer::page_state::ok:
		case templatizer::page_state::file_error:
		case templatizer::page_state::parse_error:
			this->state_ = new_state;
	}
}


// private:

// class chunk
templatizer::page::chunk::~chunk() noexcept
{}


// class raw_chunk
templatizer::page::raw_chunk::raw_chunk(const char *data, size_t size) noexcept:
	data_(data),
	size_(size)
{}

void
templatizer::page::raw_chunk::init(const templatizer::model &) const noexcept
{}

void
templatizer::page::raw_chunk::clear() const noexcept
{}


const char *
templatizer::page::raw_chunk::data() const noexcept
{ return this->data_; }

size_t
templatizer::page::raw_chunk::size() const noexcept
{ return this->size_; }

std::string
templatizer::page::raw_chunk::symbol() const noexcept
{ return ""; }


// class var_chunk
templatizer::page::var_chunk::var_chunk(const std::string &symbol) noexcept:
	symbol_(symbol),
	data_(nullptr)
{}

templatizer::page::var_chunk::var_chunk(std::string &&symbol) noexcept:
	symbol_(std::move(symbol)),
	data_(nullptr)
{}


void
templatizer::page::var_chunk::init(const templatizer::model &model) const
{
	// This can throw:
	this->data_ = &model.at(this->symbol_);
}

void
templatizer::page::var_chunk::clear() const noexcept
{
	// Do NOT delete! Model manages it.
	this->data_ = nullptr;
}


const char *
templatizer::page::var_chunk::data() const noexcept
{
	// Don't forget to init() first!
	return this->data_->c_str();
}

size_t
templatizer::page::var_chunk::size() const noexcept
{
	// Don't forget to init() first!
	return this->data_->size();
}

std::string
templatizer::page::var_chunk::symbol() const noexcept
{ return this->symbol_; }
