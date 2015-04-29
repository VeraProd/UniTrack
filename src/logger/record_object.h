// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_RECORD_OBJECT_H
#define LOGGER_RECORD_OBJECT_H

#include <sstream>
#include <string>
#include <regex>

#include <logger/level.h>


namespace logger {


class logger;


class record_object
{
public:
	record_object(level level_, logger &logger_) noexcept;
	record_object(record_object &&other) noexcept;
	~record_object();
	
	
	// Non-copy constructable or copy/move assignable.
	record_object(const record_object &other) = delete;
	record_object & operator=(const record_object &other) = delete;
	record_object & operator=(record_object &&other) = delete;
	
	
	template<class T>
	inline record_object & operator<<(T data);
private:
	// Nedd for replacing "\r" symbols
	static const std::regex regex_r_;
	static const std::string replace_r_by_;
	
	level level_;
	logger &logger_;
	std::ostringstream stream_;
	
	bool log_on_destroy_;
};


};	// logger

#include <logger/record_object.hpp>

#endif // LOGGER_RECORD_OBJECT_H
