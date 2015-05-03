// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/worker.h>

#include <functional>


server::worker::worker(logger::logger &logger,
					   const server::worker_parameters &parameters,
					   boost::asio::io_service &io_service,
					   server::hosts_manager &hosts_manager):
	logger_(logger),
	parameters_(parameters),
	
	hosts_manager_(hosts_manager),
	
	io_service_(io_service),
	work_(io_service_),
	
	worker_thread_(std::bind(&worker::run, this))
{}


// Adds new client to the worker
// Returns true, if added successfully
bool
server::worker::add_client(server::socket_ptr_t socket_ptr) noexcept
{
	this->logger_.stream(logger::level::info)
		<< "Worker: Adding client to worker " << this->id() << '.';
	
	
	try {
		auto it = this->client_managers_.emplace(this->client_managers_.end(), nullptr);
		*it = std::make_shared<server::client_manager>(this->logger_,
													   *this,
													   it,
													   socket_ptr,
													   this->hosts_manager_);
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
server::worker::erase_client(client_manager_list_const_iterator_t iterator) noexcept
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
