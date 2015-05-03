// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_TYPES_H
#define SERVER_TYPES_H

#include <memory>
#include <unordered_set>

#include <boost/asio.hpp>


namespace server {


// Sockets
typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr_t;


// Ports
typedef unsigned short				port_t;
typedef std::unordered_set<port_t>	port_set_t;


// Workers
typedef unsigned int worker_id_t;


};	// namespace server


#endif // SERVER_TYPES_H
