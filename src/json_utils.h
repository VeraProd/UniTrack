// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <string>

#include <json.hpp>


namespace utils {


nlohmann::json
json_from_file(const std::string &file_name);


};	// namespace utils


#endif // JSON_UTILS_H
