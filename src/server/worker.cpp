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
server::worker::add_client(socket_ptr_t socket_ptr)
{
	this->logger_.stream(logger::level::info)
		<< "Worker: Adding client to worker " << this->id() << '.';
	
	this->client_managers_.emplace_back(this->logger_, this->io_service_, socket_ptr);
	
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << ": Client added.";
	
	return true;
}


// private
// Must be called in worker_thread_ thread
// NOTE: Constructor calls this automatically. Do NOT call it manually!
void
server::worker::run()
{
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " started.";
	
	this->io_service_.run();	// Stops when server stops this service
	this->stop();
}


// Stops the worker
// NOTE: Do NOT call this manually! Worker's run() does it.
void
server::worker::stop()
{
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " stopping...";
	
	
	
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " stopped.";
}