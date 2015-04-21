// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include <thread>
#include <list>

#include <boost/asio.hpp>

#include <server/client_manager.h>


namespace server {


typedef unsigned int worker_id_t;


class worker
{
public:
	worker(worker_id_t id,
		   boost::asio::io_service &io_service,
		   boost::asio::ip::tcp::endpoint &endpoint);
	
	inline worker_id_t id() const
	{ return this->id_; }
protected:
	void run();
	void stop();
	
	void accept_handler(std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr,
						const boost::system::error_code &err);
	void add_accept_handler();
private:
	worker_id_t id_;
	
	boost::asio::io_service &io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;
	
	std::list<client_manager> client_managers_;
	
	std::thread worker_thread_;
};	// class worker


};	// namespace server

#endif // SERVER_WORKER_H
