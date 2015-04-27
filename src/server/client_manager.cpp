// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/client_manager.h>

#include <functional>

#include <server/worker.h>


server::client_manager::client_manager(logger::logger &logger,
									   worker &w,
									   std::list<client_manager>::const_iterator it,
									   socket_ptr_t socket_ptr):
	logger_(logger),
	
	worker_(w),
	iterator_(it),
	
	socket_ptr_(socket_ptr)
{
	using namespace std::placeholders;
	
	this->logger_.stream(logger::level::info)
		<< "Connected!!!";
	
	boost::asio::async_read_until(*this->socket_ptr_,
								  this->headers_buf_,
								  "\r\n\r\n",
								  std::bind(&client_manager::headers_handler, this, _1, _2));
}


// private
// Closes the socket and removes manager from worker
void
server::client_manager::finish_work()
{
	// Closing the socket
	{
		boost::system::error_code err;
		this->socket_ptr_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
		this->socket_ptr_->close();
	}
	
	this->worker_.erase_client(this->iterator_);
}


void
server::client_manager::headers_handler(const boost::system::error_code &err,
										size_t bytes_transferred)
{
	
}
