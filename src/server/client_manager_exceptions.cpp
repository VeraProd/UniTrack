// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/client_manager_exceptions.h>


server::bad_client::bad_client():
	std::runtime_error("Bad client")
{}
