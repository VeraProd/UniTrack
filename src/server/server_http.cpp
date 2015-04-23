// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/server_http.h>
using namespace boost::asio;

#include <functional>


server::server_http::server_http(logger::logger &logger,
								 const server_parameters &parameters):
	logger_(logger),
	parameters_(parameters),
	
	io_service_(),
	endpoint_(ip::tcp::v4(), parameters_.port),
	acceptor_(io_service_, endpoint_),
	
	current_worker_id_(0),
	
	server_thread_()
{
	this->logger_.stream(logger::level::info)
		<< "Server: Starting (port: " << this->parameters_.port
		<< ", workers: " << this->parameters_.workers << ")...";
	
	this->server_thread_ = std::move(std::thread(std::bind(&server_http::run, this)));
	
	
	// Workers creation
	try {
		this->logger_.stream(logger::level::info)
			<< "Server: Creating " << this->parameters_.workers << " workers...";
		
		auto workers_count = this->parameters_.workers;
		this->workers_.reserve(workers_count);
		
		while (workers_count--) {
			this->workers_.emplace_back(new worker(this->logger_,
												   this->current_worker_id_++,
												   this->io_service_));
		}
		
		this->logger_.stream(logger::level::info)
			<< "Server: Workers created.";
	} catch (const std::exception &e) {
		this->logger_.stream(logger::level::critical)
			<< "Server: Exception: " << e.what() << '.';
		
		this->logger_.stream(logger::level::critical)
			<< "Server: NOT started.";
		
		return;
	}
	
	this->logger_.stream(logger::level::info)
		<< "Server: Started (port: " << this->parameters_.port
		<< ", workers: " << this->parameters_.workers << ")...";
}


void
server::server_http::stop()
{
	this->logger_.stream(logger::level::info)
		<< "Server: Stopping...";
	
	boost::system::error_code err;
	this->acceptor_.close(err);
	
	// for (auto &worker_ptr: this->workers_)
	// 	worker_ptr->stop();
	
	// for (auto &worker_ptr: this->workers_)
	// 	worker_ptr->join();
	
	this->io_service_.stop();	// This must stops all workers
}


void
server::server_http::join()
{
	this->server_thread_.join();
}


void
server::server_http::detach()
{
	this->server_thread_.detach();
}


// private
void
server::server_http::run()
{
	this->logger_.stream(logger::level::info)
		<< "Server: Running...";
	
	this->add_accept_handler();
	this->io_service_.run();
	
	this->logger_.stream(logger::level::info)
		<< "Server: Stopped.";
}


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
		
		
		this->current_worker_id_ = (this->current_worker_id_ + 1) % this->workers_.size();
		
		this->logger_.stream(logger::level::info)
			<< "Server: Connection accepted, give it to worker "
			<< this->current_worker_id_ << "...";
		
		this->workers_[this->current_worker_id_]->add_client(socket_ptr);
	}
}


void
server::server_http::add_accept_handler()
{
	using namespace std::placeholders;
	
	auto socket_ptr = std::make_shared<ip::tcp::socket>(this->io_service_);
	this->acceptor_.async_accept(*socket_ptr,
								 std::bind(&server_http::accept_handler,
										   this, socket_ptr, _1));
}
