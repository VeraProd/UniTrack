// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/worker.h>
using namespace boost::asio;

#include <functional>


server::worker::worker(logger::logger &logger,
					   worker_id_t id,
					   boost::asio::io_service &io_service):
	logger_(logger),
	id_(id),
	
	io_service_(io_service),
	work_(io_service_),
	
	incoming_clients_(128),
	
	worker_thread_(std::bind(&worker::run, this))
{}


// protected
void
server::worker::run()
{
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " started.";
	
	this->io_service_.run();
	
	this->logger_.stream(logger::level::info)
		<< "Worker: Worker " << this->id() << " stopped.";
}


void
server::worker::stop()
{
	this->logger_.stream(logger::level::info)
		<< "Worker: Stopping worker " << this->id() << "...";
	
	this->io_service_.stop();
}


bool
server::worker::add_client(socket_ptr_t socket_ptr)
{
	this->logger_.stream(logger::level::info)
		<< "Worker: Client added to worker " << this->id() << '.';
	
	// this->client_managers_.emplace_back(this->io_service_, socket_ptr);
	return this->incoming_clients_.push(socket_ptr);
}
