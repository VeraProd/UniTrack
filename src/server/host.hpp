// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
const std::string &
server::host::name() const noexcept
{
	return this->host_parameters_.name;
}
