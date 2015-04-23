// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/worker.h>

#include <functional>


server::worker::worker(logger::logger &logger,
					   worker_id_t id,
					   boost::asio::io_service &io_service):
	logger_(logger),
	id_(id),
	
	io_service_(io_service),
	
	incoming_clients_(128),
	
	worker_thread_(std::bind(&worker::run, this))
{}


// Adds new client to the worker
// Returns true, if added successfully
bool
server::worker::add_client(socket_ptr_t socket_ptr)
{
	this->logger_.stream(logger::level::info)
		<< "Worker: Adding client to worker " << this->id() << '.';
	
	// this->client_managers_.emplace_back(this->io_service_, socket_ptr);
	if (this->incoming_clients_.push(socket_ptr)) {
		this->logger_.stream(logger::level::info)
			<< "Worker: Client added to worker " << this->id() << '.';
		return true;
	} else {
		this->logger_.stream(logger::level::error)
			<< "Worker: Unable to add client to worker " << this->id()
			<< ": Client quque is full.";
		return false;
	}
}


// Joins worker's thread
void
server::worker::join()
{
	this->worker_thread_.join();
}


// Detaches worker's thread
void
server::worker::detach()
{
	this->worker_thread_.detach();
}


// private
// Must be called in worker_thread_ thread
void
server::worker::run()
{
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " started.";
	
	this->io_service_.run();	// Stops when server stops this service
	
	this->stop();
}


// Stops the worker
void
server::worker::stop()
{
	// this->io_service_ must be stopped from the server thread!
	
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " stopping...";
	
	{
		socket_ptr_t socket_ptr;
		
		while (this->incoming_clients_.pop(socket_ptr)) {
			boost::system::error_code err;
			socket_ptr->shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
			socket_ptr->close(err);
			socket_ptr.reset();
		}
	}
	
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " stopped.";
}
