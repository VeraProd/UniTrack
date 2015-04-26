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
	
	// incoming_clients_(parameters_.max_incoming_clients),
	
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
	
	// if (this->incoming_clients_.push(socket_ptr)) {
	// 	this->logger_.stream(logger::level::info)
	// 		<< "Worker: Client added to worker " << this->id() << '.';
	// 	return true;
	// } else {
	// 	this->logger_.stream(logger::level::error)
	// 		<< "Worker: Unable to add client to worker " << this->id()
	// 		<< ": Client queue is full.";
	// 	return false;
	// }
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
	
	{
		socket_ptr_t socket_ptr;
		
		// while (this->incoming_clients_.pop(socket_ptr)) {
		// 	boost::system::error_code err;
		// 	socket_ptr->shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
		// 	socket_ptr->close(err);
		// }
	}
	
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " stopped.";
}
