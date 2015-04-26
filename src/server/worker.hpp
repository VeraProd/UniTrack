// Author: Dmitry Kukovinets (d1021976@gmail.com)


// Checks worker's thread for joinable
inline bool
server::worker::joinable() const
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
