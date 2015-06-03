// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_FILE_HOST_PARAMETERS_H
#define SERVER_FILE_HOST_PARAMETERS_H

#include <string>
#include <vector>
#include <regex>

#include <json.hpp>

#include <server/host_parameters.h>


namespace server {


struct file_host_only_parameters
{
	// May be useful, if several allow_regexes specified.
	enum class allow_match_mode
	{
		any,	// Uri matches any of allow_regexes (and doesn't match any of deny_regexes).
		all		// Uri matches all of allow_regexes (and doesn't match any of deny_regexes).
	};
	
	
	std::string root;						// Required
	
	std::vector<std::regex>
		deny_regexes =						// Optional
			{
				std::regex(".*\\.\\./.*"),	// Don't allow "../" sequences!
				std::regex(".*/\\..*")		// Don't allow hidden directories and files
			},
		allow_regexes =						// Optional
			{
				// Don't allow anything by default
			};
	
	allow_match_mode mode;					// Reqired
	
	
	explicit file_host_only_parameters() = default;
	explicit file_host_only_parameters(const nlohmann::json &config);
};	// struct file_host_only_parameters


struct file_host_parameters:
	public host_parameters,
	public file_host_only_parameters
{
	using file_host_only_parameters::allow_match_mode;
	
	
	explicit file_host_parameters():
		host_parameters(),
		file_host_only_parameters()
	{}
	
	explicit file_host_parameters(const nlohmann::json &config):
		host_parameters(config),
		file_host_only_parameters(config)
	{}
};	// struct file_host_parameters


};	// namespace server


#endif // SERVER_FILE_HOST_PARAMETERS_H
