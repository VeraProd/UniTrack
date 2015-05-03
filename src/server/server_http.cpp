// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/server_http.h>

#include <functional>
#include <system_error>


server::server_http::server_http(logger::logger &logger,
								 const server_parameters &parameters):
	logger_(logger),
	parameters_(parameters),
	
	hosts_manager_(logger_),
	
	acceptors_io_service_(),
	acceptor_empty_work_(acceptors_io_service_),
	
	workers_io_service_(),
	
	server_thread_()
{
	try {
		// Acceptors creation
		{
			for (auto port: this->parameters_.ports) {
				try {
					this->acceptor_ptrs_.emplace_back(
						new server::acceptor(this->logger_,
											 *this,
											 { .port = port },
											 this->acceptors_io_service_,
											 this->workers_io_service_));
				} catch (const boost::system::system_error &e) {
					this->logger_.stream(logger::level::error)
						<< "Server: Accepting on port: " << port
						<< ": NOT started: " << e.what() << '.';
				}
			}
			
			this->logger_.stream(logger::level::info)
				<< "Server: Acceptors created.";
		}
		
		
		// Workers creation
		{
			auto workers_count = this->parameters_.workers;
			this->worker_ptrs_.reserve(workers_count);
			
			worker_parameters parameters;
			
			worker_id_t current_worker_id = 0;
			while (workers_count--) {
				parameters.id = current_worker_id++;
				
				std::unique_ptr<server::worker> worker_ptr(
					new server::worker(this->logger_,
									   parameters,
									   this->workers_io_service_,
									   this->hosts_manager()));
				
				this->workers_dispatch_table_.emplace(worker_ptr->thread_id(), worker_ptr->id());
				this->worker_ptrs_.emplace_back(std::move(worker_ptr));
			}
			
			this->logger_.stream(logger::level::info)
				<< "Server: " << this->parameters_.workers << " workers created.";
		}
		
		
		// Server starting
		this->server_thread_ = std::move(std::thread(std::bind(&server_http::run, this)));
	} catch (const std::exception &e) {
		this->logger_.stream(logger::level::critical)
			<< "Server: Exception: " << e.what() << '.';
		
		this->logger_.stream(logger::level::critical)
			<< "Server: NOT started.";
	}
}


void
server::server_http::stop() noexcept
{
	this->logger_.stream(logger::level::info)
		<< "Server: Stopping...";
	
	this->acceptors_io_service_.stop();	// Stopping acceptors
	this->workers_io_service_.stop();	// Stopping all workers
	
	
	// Waiting for workers
	this->logger_.stream(logger::level::info)
		<< "Server: Waiting for workers...";
	
	for (auto &worker_ptr: this->worker_ptrs_) {
		try {
			worker_ptr->join();
		} catch (const std::system_error &e) {
			this->logger_.stream(logger::level::error)
				<< "Server: Unable to join to worker " << worker_ptr->id() << " thread: "
				<< e.what() << '.';
		}
	}
	
	
	// Waiting for server
	try {
		this->join();
	} catch (const std::system_error &e) {
		this->logger_.stream(logger::level::error)
			<< "Server: Unable to join to server thread: " << e.what() << '.';
	}
}


// protected
// Dispatches client pointed by socket_ptr to one of worker threads
// Only acceptor calls this!
void
server::server_http::dispatch_client(server::socket_ptr_t socket_ptr) noexcept
{
	this->workers_io_service_.dispatch(std::bind(&server_http::dispatch_client_worker_thread,
												 this, socket_ptr));
}


// private
// Dispatches client to one of workers
// WARNING: this method calls from one of workers' threads, NOT from server thread!
// Only dispatch_client() calls this!
void
server::server_http::dispatch_client_worker_thread(server::socket_ptr_t socket_ptr) noexcept
{
	auto current_thread_id = std::this_thread::get_id();
	try {
		auto worker_id = this->workers_dispatch_table_.at(current_thread_id);
		auto &worker_ptr = this->worker_ptrs_[worker_id];
		worker_ptr->add_client(socket_ptr);
	} catch (const std::out_of_range &) {	// Impossible
		this->logger_.stream(logger::level::error)
			<< "Server: Invalid worker thread dispatched: "
			<< ((current_thread_id == this->server_thread_.get_id())?
				"server thread.": "unknown thread.");
	} catch (...) {							// Impossible too
		this->logger_.stream(logger::level::error)
			<< "Server: Unable to add client to worker. Unknown error.";
	}
}


void
server::server_http::run() noexcept
{
	{
		auto stream = std::move(this->logger_.stream(logger::level::info));
		stream << "Server: Started (ports:";
		for (auto port: this->parameters_.ports)
			stream << ' ' << port << ',';
		stream << " workers: " << this->parameters_.workers << ").";
	}
	
	boost::system::error_code err;
	this->acceptors_io_service_.run(err);
	
	if (err) {
		this->logger_.stream(logger::level::error)
			<< "Server: Stopped with error: " << err.message() << '.';
	} else {
		this->logger_.stream(logger::level::info)
			<< "Server: Stopped.";
	}
}
