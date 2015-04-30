// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
const std::string &
server::client_manager::client_ip_address() const
{
	return this->client_ip_address_;
}


inline
bool
server::client_manager::keep_alive() const
{
	return this->connection_params_.keep_alive;
}


inline
void
server::client_manager::keep_alive(bool status)
{
	this->connection_params_.keep_alive = status;
}


// protected
template<class Exception>
inline
void
server::client_manager::log_error(const Exception &e, const server::http::status &status)
{
	this->log_error(e.what(), status);
}


template<class Exception>
inline
void
server::client_manager::handle_error(const Exception &e,
									 const server::http::status &status,
									 bool exit,
									 bool send_phony,
									 std::unordered_map<std::string, std::string> &&headers)
{
	this->handle_error(e.what(), status, exit, send_phony, std::move(headers));
}
