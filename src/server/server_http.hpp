// Author: Dmitry Kukovinets (d1021976@gmail.com)


// Checks server's thread for joinable
inline bool
server::server_http::joinable() const noexcept
{
	return this->server_thread_.joinable();
}


// Joins server's thread
inline void
server::server_http::join()
{
	this->server_thread_.join();
}


// Detaches server's thread
inline void
server::server_http::detach()
{
	this->server_thread_.detach();
}


// Returns the hosts manager of this server
inline
server::host_manager &
server::server_http::host_manager() noexcept
{
	return this->host_manager_;
}
