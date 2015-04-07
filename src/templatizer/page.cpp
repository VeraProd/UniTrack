// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <templatizer/page.h>

#include <fstream>
#include <regex>

#include <templatizer/module_registrar.h>
#include <templatizer/modules/var_chunk.h>


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
			
			
			// REGEX: ${command ARGUMENTS}	-- full form
			//    or: $VAR_NAME				-- short form, equvivalents to: $(var VAR_NAME)
			// VAR_NAME can contain characters: a-z, A-Z, 0-9 and '_', but must to begin with
			// letter or '_' (like in C). Minimum length of name is 1 symbol.
			// NOTE: Some space symbols around VAR_NAME is allowed (but not a newline).
			// VAR_NAME is a first group of the regex.
			static const std::regex regex(
					// Full form: ${ COMMAND ... }
					"\\${[[:blank:]]*\\([[:alpha:]_][[:alnum:]_]*\\)"	// Group 1: COMMAND
						"[[:blank:]]*\\([^}]*\\)}"						// Group 2: ARG
				"\n"	// 'Or' in grep mode
					// Short form: $VAR_NAME
					"\\$\\([[:alpha:]_][[:alnum:]_]*\\)",				// Group 3: VAR_NAME (in short form)
				std::regex::grep | std::regex::optimize);
			
			
			std::regex_iterator<const char *>
				it(region_data, region_data + region_size, regex),
				end;
			
			size_t old_pos = 0;
			while (it != end) {
				// Adding previous raw chunk
				size_t current_pos = it->position();
				if (current_pos > old_pos) {	// Indexing previous raw chunk...
					chunks.emplace_back(new raw_chunk(region_data + old_pos, current_pos - old_pos));
					old_pos = current_pos + it->length();
				}
				
				
				// Adding current extra chunk
				std::string command  = std::move(it->str(1)),
							argument = std::move(it->str(2));
				if (it->length(1) == 0) {	// Short form (because of command is empty)
					command  = templatizer::var_chunk::var_chunk_cmd;
					argument = std::move(it->str(3));
				}
				
				auto chunk_generator = std::move(templatizer::default_module_registrar.at(command));
				chunks.emplace_back(std::move(chunk_generator(std::move(argument))));
				
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
	} catch (std::out_of_range &e) {
		std::cerr << "Can't parse file: \"" << file << "\": " << e.what() << std::endl;
		this->set_state(templatizer::page_state::parse_error);
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
void
templatizer::page::generate(std::ostream &stream, const templatizer::model &model) const
{
	for (const auto &chunk: this->chunks_)
		chunk->generate(stream, model);
}


// All symbols need to get from model
templatizer::page::symbol_set
templatizer::page::symbols() const
{
	templatizer::page::symbol_set res;
	this->export_symbols(res);
	return res;
}


// Same as symbols(), but puts them into set
void
templatizer::page::export_symbols(templatizer::page::symbol_set &symbols) const
{
	for (const auto &chunk: this->chunks_)
		chunk->export_symbols(symbols);
}


// State
void
templatizer::page::set_state(int new_state)
{ this->state_ = new_state; }
