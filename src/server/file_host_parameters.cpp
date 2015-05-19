// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/file_host_parameters.h>


server::file_host_only_parameters::file_host_only_parameters(const nlohmann::json &config):
	root(config.at("root").get<decltype(this->root)>())
{
	// Allow mode
	{
		const auto &mode = config.at("allow_match_mode").get<std::string>();
		if (mode == "any") {
			this->mode = server::file_host_parameters::allow_match_mode::any;
		} else if (mode == "all") {
			this->mode = server::file_host_parameters::allow_match_mode::all;
		} else {
			throw std::logic_error("Incorrect config");
		}
	}
	
	
	// Optional parameters
	try {
		const auto &regexes = config.at("allow_regexes");
		if (!regexes.empty()) {
			this->allow_regexes.clear();
			for (const std::string &regex: regexes)
				this->allow_regexes.push_back(std::regex(regex));
		}
	} catch (const std::out_of_range &) {}
	
	
	try {
		const auto &regexes = config.at("deny_regexes");
		if (!regexes.empty()) {
			this->deny_regexes.clear();
			for (const std::string &regex: regexes)
				this->deny_regexes.push_back(std::regex(regex));
		}
	} catch (const std::out_of_range &) {}
}
