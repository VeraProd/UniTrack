// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <json_utils.h>

#include <fstream>


nlohmann::json
utils::json_from_file(const std::string &file_name)
{
	nlohmann::json json;
	std::ifstream stream(file_name.c_str());
	json << stream;
	return json;
}
