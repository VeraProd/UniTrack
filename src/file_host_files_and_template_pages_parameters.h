// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef FILE_HOST_FILES_AND_TEMPLATE_PAGES_PARAMETERS_H
#define FILE_HOST_FILES_AND_TEMPLATE_PAGES_PARAMETERS_H

#include <regex>

#include <json.hpp>


struct files_and_template_pages_parameters
{
	enum class behavior {
		template_pages,
		files
	};
	
	
	// By default: interpret all files as plain files...
	behavior default_behavior = behavior::files;				// Optional
	
	// ...but change behavior for .html files.
	std::regex change_behavior_regex = std::regex(".*\\.html");	// Optional
	
	
	explicit files_and_template_pages_parameters() {}
	explicit files_and_template_pages_parameters(const nlohmann::json &config);
};	// struct files_and_template_pages_parameters


#endif // FILE_HOST_FILES_AND_TEMPLATE_PAGES_PARAMETERS_H
