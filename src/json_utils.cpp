// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <json_utils.h>

#include <fstream>
#include <stdexcept>


nlohmann::json
utils::json_from_file(const std::string &file_name)
{
	std::ifstream stream(file_name.c_str());
	if (stream.bad() || stream.fail())
		throw std::logic_error("Can't open config file: \"" + file_name + '\"');
	
	nlohmann::json json;
	json << stream;
	return json;
}
