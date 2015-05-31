// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/logger.h>


logger::logger::logger(std::ostream &log_stream):
	log_stream_(log_stream)
{
	this->stream(level::info)
		<< "Log started...";
}


logger::logger::~logger()
{
	this->stream(level::info)
		<< "End of log.";
	
	std::unique_lock<std::mutex> lock(this->log_stream_mutex_);
	this->log_stream_.flush();
}


logger::record_object
logger::logger::stream(level level_) noexcept
{
	return record_object(*this, level_);
}


void
logger::logger::log_raw(level level_, const std::string &data)
{
	const auto &level_str = level_to_str(level_);
	
	std::unique_lock<std::mutex> lock(this->log_stream_mutex_);
	this->log_stream_ << level_str << ' ' << data << std::endl;
}
