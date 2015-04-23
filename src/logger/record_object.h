// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_RECORD_OBJECT_H
#define LOGGER_RECORD_OBJECT_H

#include <sstream>
#include <string>

#include <logger/level.h>


namespace logger {

class logger;


class record_object
{
public:
	record_object(level level_, logger &logger_);
	record_object(record_object &&other);
	~record_object();
	
	template<class T>
	inline record_object & operator<<(const T &data)
	{
		this->stream_ << data;
		return *this;
	}
private:
	level level_;
	logger &logger_;
	std::stringstream stream_;
	
	bool log_on_destroy_;
};


};	// logger

#endif // LOGGER_RECORD_OBJECT_H
