// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/worker.h>

#include <functional>


server::worker::worker(logger::logger &logger,
					   const server::worker_parameters &parameters,
					   boost::asio::io_service &io_service):
	logger_(logger),
	parameters_(parameters),
	
	io_service_(io_service),
	work_(io_service_),
	
	worker_thread_(std::bind(&worker::run, this))
{}


// Adds new client to the worker
// Returns true, if added successfully
bool
server::worker::add_client(socket_ptr_t socket_ptr) noexcept
{
	this->logger_.stream(logger::level::info)
		<< "Worker: Adding client to worker " << this->id() << '.';
	
	
	try {
		this->client_managers_.emplace_back(this->logger_,
											*this,
											this->client_managers_.end(),
											socket_ptr);
	} catch (...) {
		this->logger_.stream(logger::level::error)
			<< "Worker: Worker " << this->id() << ": Client not added.";
		
		return false;
	}
	
	
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << ": Client added.";
	
	return true;
}


// Erases client by iterator. Client manager uses this.
void
server::worker::erase_client(std::list<client_manager>::const_iterator iterator) noexcept
{
	this->client_managers_.erase(iterator);
}


// private
// Must be called in worker_thread_ thread
// NOTE: Constructor calls this automatically. Do NOT call it manually!
void
server::worker::run() noexcept
{
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " started.";
	
	this->io_service_.run();	// Stops when server stops this service
	this->stop();
}


// Stops the worker
// NOTE: Do NOT call this manually! Worker's run() does it.
void
server::worker::stop() noexcept
{
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " stopping...";
	
	
	
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " stopped.";
}
