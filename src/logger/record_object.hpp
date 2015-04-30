// Author: Dmitry Kukovinets (d1021976@gmail.com)

namespace logger {


template<class T>
inline
record_object &
record_object::operator<<(T data)
{
	this->stream_ << data;
	return *this;
}


template<>
inline
record_object &
record_object::operator<<(const std::string &data)
{
	this->stream_ << std::regex_replace(data, this->regex_r_, this->replace_r_by_);
	return *this;
}


template<>
inline
record_object &
record_object::operator<<(std::string &&data)
{
	this->stream_ << std::regex_replace(data, this->regex_r_, this->replace_r_by_);
	return *this;
}


template<>
inline
record_object &
record_object::operator<<(char const * const data)
{
	this->stream_ << std::regex_replace(data, this->regex_r_, this->replace_r_by_);
	return *this;
}


template<>
inline
record_object &
record_object::operator<<(char *data)
{
	this->stream_ << std::regex_replace(data, this->regex_r_, this->replace_r_by_);
	return *this;
}


template<>
inline
record_object &
record_object::operator<<(char data)
{
	if (data != '\r')
		this->stream_ << data;
	return *this;
}


};	// namespace logger
