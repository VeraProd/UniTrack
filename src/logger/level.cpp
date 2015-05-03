// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/level.h>


namespace {


const std::string
	// Base levels
	level_info			= "info        ",
	level_warning		= "warning     ",
	level_error			= "error       ",
	level_critical		= "critical    ",
	level_other			= "other       ",
	
	// Security levels
	level_sec_info		= "sec_info    ",
	level_sec_warning	= "sec_warning ",
	level_sec_error		= "sec_error   ",
	level_sec_critical	= "sec_critical",
	level_sec_other		= "sec_other   ",
	
	// Unknown level
	level_unknown		= "unknown     ";


};


const std::string &
logger::level_to_str(logger::level level_) noexcept
{
	switch (level_) {
		case logger::level::info:			return level_info;
		case logger::level::warning:		return level_warning;
		case logger::level::error:			return level_error;
		case logger::level::critical:		return level_critical;
		
		case logger::level::sec_info:		return level_sec_info;
		case logger::level::sec_warning:	return level_sec_warning;
		case logger::level::sec_error:		return level_sec_error;
		case logger::level::sec_critical:	return level_sec_critical;
		
		case logger::level::other:			return level_other;
		
		default:							return level_unknown;
	}
}
