// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/level.h>


namespace {

const std::string
	level_info		= "info    ",
	level_warning	= "warning ",
	level_error		= "error   ",
	level_critical	= "critical",
	level_other		= "other   ",
	level_unknown	= "unknown ";

};


const std::string &
logger::level_to_str(level level_)
{
	switch (level_) {
		case level::info:		return level_info;
		case level::warning:	return level_warning;
		case level::error:		return level_error;
		case level::critical:	return level_critical;
		case level::other:		return level_other;
		default:				return level_unknown;
	}
}
