// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_CLIENT_MANAGER_EXCEPTIONS_H
#define SERVER_CLIENT_MANAGER_EXCEPTIONS_H

#include <stdexcept>


namespace server {


class bad_client: public std::runtime_error
{
public:
	explicit bad_client();
};


};	// namespace server


#endif // SERVER_CLIENT_MANAGER_EXCEPTIONS_H
