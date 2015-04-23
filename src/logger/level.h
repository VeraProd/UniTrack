// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_LEVEL_H
#define LOGGER_LEVEL_H

#include <string>

namespace logger {


enum class level
{
	info,
	warning,
	error,
	critical,
	other
};


const std::string & level_to_str(level level_);


};	// logger

#endif // LOGGER_LEVEL_H
