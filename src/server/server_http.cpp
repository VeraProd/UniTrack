// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/server_http.h>
using namespace boost::asio;

#include <functional>


server::server_http::server_http(logger::logger &logger,
								 const server_parameters &parameters):
	logger_(logger),
	parameters_(parameters),
	
	server_io_service_(),
	endpoint_(ip::tcp::v4(), parameters_.port),
	acceptor_(server_io_service_, endpoint_),
	
	current_worker_id_(0),
	
	server_thread_()
{
	try {
		// Workers creation
		{
			this->logger_.stream(logger::level::info)
				<< "Server: Creating " << this->parameters_.workers << " workers...";
			
			auto workers_count = this->parameters_.workers;
			this->worker_ptrs_.reserve(workers_count);
			
			worker_parameters parameters;
			parameters.max_incoming_clients	= this->parameters_.worker_max_incoming_clients;
			parameters.max_clients			= this->parameters_.worker_max_clients;
			
			while (workers_count--) {
				parameters.id = this->current_worker_id_++;
				
				std::unique_ptr<worker> worker_ptr(new worker(this->logger_,
															  parameters,
															  this->workers_io_service_));
				
				this->workers_dispatch_table_.emplace(worker_ptr->thread_id(), worker_ptr->id());
				this->worker_ptrs_.emplace_back(std::move(worker_ptr));
			}
			this->current_worker_id_ = 0;
			
			this->logger_.stream(logger::level::info)
				<< "Server: Workers created.";
		}
		
		
		// Server starting
		this->logger_.stream(logger::level::info)
			<< "Server: Starting (port: " << this->parameters_.port
			<< ", workers: " << this->parameters_.workers << ")...";
		
		this->server_thread_ = std::move(std::thread(std::bind(&server_http::run, this)));
	} catch (const std::exception &e) {
		this->logger_.stream(logger::level::critical)
			<< "Server: Exception: " << e.what() << '.';
		
		this->logger_.stream(logger::level::critical)
			<< "Server: NOT started.";
	}
}


void
server::server_http::stop()
{
	this->logger_.stream(logger::level::info)
		<< "Server: Stopping...";
	
	this->server_io_service_.stop();
	this->workers_io_service_.stop();	// Stopping all workers
	
	boost::system::error_code err;
	this->acceptor_.close(err);
	
	// Waiting for workers
	this->logger_.stream(logger::level::info)
		<< "Server: Waiting for workers...";
	
	for (auto &worker_ptr: this->worker_ptrs_)
		worker_ptr->join();
}


// private
void
server::server_http::run()
{
	this->add_accept_handler();
	
	this->logger_.stream(logger::level::info)
		<< "Server: Started (port: " << this->parameters_.port
		<< ", workers: " << this->parameters_.workers << ").";
	
	this->server_io_service_.run();
	
	this->logger_.stream(logger::level::info)
		<< "Server: Stopped.";
}


// Handles the accept event
void
server::server_http::accept_handler(socket_ptr_t socket_ptr,
									const boost::system::error_code &err)
{
	if (err) {
		if (err == boost::system::errc::operation_canceled) {
			this->logger_.stream(logger::level::info)
				<< "Server: Accepting stopped.";
		} else {
			this->logger_.stream(logger::level::critical)
				<< "Server: Accepting socket error: " << err.message();
		}
	} else {
		// Continue accepting...
		this->add_accept_handler();
		
		
		this->current_worker_id_ = (this->current_worker_id_ + 1) % this->worker_ptrs_.size();
		
		this->logger_.stream(logger::level::info)
			<< "Server: Connection accepted";
		
		// this->worker_ptrs_[this->current_worker_id_]->add_client(socket_ptr);
		this->workers_io_service_.dispatch(std::bind(&server_http::dispatch_client, this, socket_ptr));
	}
}


// Add accept_handler to the io_service event loop
void
server::server_http::add_accept_handler()
{
	using namespace std::placeholders;
	
	auto socket_ptr = std::make_shared<ip::tcp::socket>(this->workers_io_service_);
	this->acceptor_.async_accept(*socket_ptr,
								 std::bind(&server_http::accept_handler,
										   this, socket_ptr, _1));
}


// Dispatches client to one of workers
// WARNING: this method calls from one of workers' threads, NOT from server thread!
// Only accept_handler() call this.
void
server::server_http::dispatch_client(socket_ptr_t socket_ptr) noexcept
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
