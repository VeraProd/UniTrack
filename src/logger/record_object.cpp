// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <logger/record_object.h>

#include <ctime>

#include <logger/logger.h>


const std::regex logger::record_object::regex_r_("\r", std::regex::optimize);
const std::string logger::record_object::replace_r_by_("");


logger::record_object::record_object(level level_, logger &logger_) noexcept:
	level_(level_),
	logger_(logger_),
	
	log_on_destroy_(true)
{
	// Printing timestamp in format: DD.MM.YYYY hh:mm:ss
	time_t raw_time;
	::time(&raw_time);
	
	auto *time_info = ::localtime(&raw_time);
	char time_str[23];
	::strftime(time_str, 23, " %d.%m.%Y %H:%M:%S  ", time_info);
	
	*this << time_str;
}


logger::record_object::record_object(record_object &&other) noexcept:
	level_(other.level_),
	logger_(other.logger_),
	stream_(std::move(other.stream_)),
	
	log_on_destroy_(true)
{
	other.log_on_destroy_ = false;
}


logger::record_object::~record_object()
{
	if (this->log_on_destroy_)
		this->logger_.log_raw(this->level_, this->stream_.str());
}
