// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_LEVEL_H
#define LOGGER_LEVEL_H

#include <string>

namespace logger {


enum class level
{
	// Base levels
	info,
	warning,
	error,
	critical,
	other,
	
	// Security levels
	sec_info,
	sec_warning,
	sec_error,
	sec_critical,
	sec_other
};


const std::string & level_to_str(level level_) noexcept;


};	// logger

#endif // LOGGER_LEVEL_H
