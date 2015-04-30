// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
unsigned int
server::http::status::code() const noexcept
{
	return this->code_;
}


inline
const std::string &
server::http::status::code_str() const noexcept
{
	return this->code_str_;
}


inline
const std::string &
server::http::status::description() const noexcept
{
	return this->description_;
}
