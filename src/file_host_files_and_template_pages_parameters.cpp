// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <file_host_files_and_template_pages_parameters.h>


files_and_template_pages_parameters::files_and_template_pages_parameters(const nlohmann::json &config)
{
	// Optional parameters
	try {
		const auto &default_behavior =
			config.at("default_behavior").get<std::string>();
		
		if (default_behavior == "files") {
			this->default_behavior =
				files_and_template_pages_parameters::behavior::files;
		} else if (default_behavior == "template_pages") {
			this->default_behavior =
				files_and_template_pages_parameters::behavior::template_pages;
		} else {
			throw std::logic_error("Incorrect config");
		}
	} catch (const std::out_of_range &) {}
	
	
	try {
		this->change_behavior_regex =
			config.at("change_behavior_regex").get<std::string>();
	} catch (const std::out_of_range &) {}
}
