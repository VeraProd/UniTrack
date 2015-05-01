// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOST_MANAGER_H
#define SERVER_HOST_MANAGER_H

#include <map>

#include <logger/logger.h>
#include <server/host.h>


namespace server {


class host_manager
{
public:
	host_manager(logger::logger &logger);
	
	
	// Adds new host with specifed logger or this host_manager's logger.
	// Returns true, if host successfully addes, or false, if host with the
	// same name is already managed by this manager (or allocation error occured).
	bool add_host(logger::logger &logger, const server::host_parameters &parameters) noexcept;
	bool add_host(const server::host_parameters &parameters) noexcept;
	
	
	// Finds the host with specified name.
	// Returns std::shared_ptr to it or throws server::host_not_found exception.
	server::host_ptr_t host(const std::string &name, unsigned int port);
private:
	logger::logger &logger_;
	
	
	class hosts_map_cmp: public std::less<std::string>
	{
	public:
		inline bool operator()(const std::string *a, const std::string *b) const
		{ return std::less<std::string>::operator()(*a, *b); }
	};
	
	std::map<const std::string *, server::host_ptr_t, hosts_map_cmp> hosts_;
};	// class host_manager


};	// namespace server


#endif // SERVER_HOST_MANAGER_H
