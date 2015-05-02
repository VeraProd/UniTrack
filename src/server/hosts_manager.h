// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_HOSTS_MANAGER_H
#define SERVER_HOSTS_MANAGER_H

#include <map>

#include <logger/logger.h>
#include <server/host.h>


namespace server {


class hosts_manager
{
public:
	hosts_manager(logger::logger &logger);
	
	
	// Adds the new host.
	// Returns true, if host successfully addes, or false, if host with the
	// same name is already managed by this manager.
	// Thread-safety: no (it's write function).
	bool add_host(server::host_ptr_t host_ptr) noexcept;
	
	
	// Finds the host with specified name.
	// Returns std::shared_ptr to it or throws server::host_not_found exception.
	// Thread-sefety: yes (it's read function).
	server::host_ptr_t host(const std::string &name, server::port_t port);
private:
	logger::logger &logger_;
	
	
	class hosts_map_cmp: public std::less<std::string>
	{
	public:
		inline bool operator()(const std::string *a, const std::string *b) const
		{ return std::less<std::string>::operator()(*a, *b); }
	};
	
	std::map<const std::string *, server::host_ptr_t, hosts_map_cmp> hosts_;
};	// class hosts_manager


};	// namespace server


#endif // SERVER_HOSTS_MANAGER_H
