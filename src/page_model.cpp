// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <page_model.h>


// static
const std::string page_model::unknown_var_ = "";


page_model::page_model(logger::logger &logger):
	logger::enable_logger(logger)
{}


const std::string &
page_model::variable(const std::string &var_name) const
{
	try {
		return this->std::unordered_map<std::string, std::string>::at(var_name);
	} catch (...) {
		this->logger().stream(logger::level::error)
			<< "Page model: Requested unknown variable: \"" << var_name << "\".";
		return page_model::unknown_var_;
	}
}
