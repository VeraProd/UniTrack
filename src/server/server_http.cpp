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
	signal_set_(io_service_, SIGTERM, SIGINT, SIGQUIT),
	
	current_worker_id_(0)
{
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
}


void
server::server_http::run()
{
	using namespace std::placeholders;
	
	this->signal_set_.async_wait(std::bind(&server_http::signal_handler, this, _1, _2));
	
	this->logger_.stream(logger::level::info)
		<< "Server: Running...";
	
	this->io_service_.run();
	
	this->logger_.stream(logger::level::info)
		<< "Server: Stopped.";
}


// private
void
server::server_http::stop()
{
	this->io_service_.stop();
}


void
server::server_http::signal_handler(const boost::system::error_code &err,
									int signal_number)
{
	if (err) {
		this->logger_.stream(logger::level::error)
			<< "Server: Caught exception (signal: " << signal_number << "): " << err.message();
	} else {
		this->logger_.stream(logger::level::info)
			<< "Server: Stopping (signal: " << signal_number << ")...";
		this->stop();
	}
}


void
server::server_http::accept_handler(socket_ptr_t socket_ptr,
									const boost::system::error_code &err)
{
	// Continue accepting...
	this->add_accept_handler();
	
	if (err) {
		this->logger_.stream(logger::level::critical)
			<< "Server: Accepting socket error: " << err.message();
	} else {
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
