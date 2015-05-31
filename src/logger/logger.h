// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <iostream>
#include <mutex>

#include <logger/level.h>
#include <logger/record_object.h>


namespace logger {


class logger
{
public:
	logger(std::ostream &log_stream);
	~logger();
	
	
	// Non-copy/-move constructable/assignable. Use ptrs.
	logger(const logger &other) = delete;
	logger(logger &&other) = delete;
	
	logger & operator=(const logger &other) = delete;
	logger & operator=(logger &&other) = delete;
	
	
	record_object stream(level level_) noexcept;
	
	void log_raw(level level_, const std::string &data);
private:
	std::ostream &log_stream_;
	std::mutex log_stream_mutex_;
};	// class logger


class enable_logger
{
public:
	enable_logger(class logger &logger) noexcept:
		logger_ptr_(&logger)
	{}
	
	enable_logger(const enable_logger &other) = default;
	enable_logger(enable_logger &&other) = default;
	
	enable_logger & operator=(const enable_logger &other) = default;
	enable_logger & operator=(enable_logger &&other) = default;
	
	
	inline class logger & logger() const noexcept
	{
		return *this->logger_ptr_;
	}
private:
	class logger *logger_ptr_;
};	// class enable_logger


};	// namespace logger


#endif // LOGGER_LOGGER_H
