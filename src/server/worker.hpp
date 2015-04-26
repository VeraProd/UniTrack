// Author: Dmitry Kukovinets (d1021976@gmail.com)


// Returns worker id
inline
server::worker_id_t
server::worker::id() const noexcept
{
	return this->parameters_.id;
}


// Returns worker thread id (need for server's dispatcher)
inline
std::thread::id
server::worker::thread_id() const noexcept
{
	return this->worker_thread_.get_id();
}


// Checks worker's thread for joinable
inline bool
server::worker::joinable() const noexcept
{
	return this->worker_thread_.joinable();
}


// Joins worker's thread
inline void
server::worker::join()
{
	this->worker_thread_.join();
}


// Detaches worker's thread
inline void
server::worker::detach()
{
	this->worker_thread_.detach();
}
