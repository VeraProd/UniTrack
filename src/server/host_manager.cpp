// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host_manager.h>

#include <new>

#include <server/host_exceptions.h>


server::host_manager::host_manager(logger::logger &logger):
	logger::enable_logger(logger)
{}


// Adds the new host.
// Returns true, if host successfully addes, or false, if host with the
// same name is already managed by this manager.
// Thread-safety: no (it's write function).
bool
server::host_manager::add_host(server::host_ptr_t host_ptr) noexcept
{
	if (host_ptr == nullptr) {
		this->logger().stream(logger::level::error)
			<< "Host manager: A nonexistent host can not be added: User, are you kidding?";
		return false;
	}
	
	auto p = this->hosts_.emplace(&host_ptr->name(), host_ptr);
	if (p.second) {
		this->logger().stream(logger::level::info)
			<< "Host manager: Host \"" << host_ptr->name()
			<< "\" added.";
	} else {
		this->logger().stream(logger::level::error)
			<< "Host manager: Host \"" << host_ptr->name()
			<< "\" not added: Host already exists.";
	}
	
	return p.second;
}


// Finds the host with specified name.
// Returns std::shared_ptr to it or throws server::host_not_found exception.
// Thread-sefety: yes (it's read function).
server::host_ptr_t
server::host_manager::host(const std::string &name, server::port_t port)
{
	auto it = this->hosts_.find(&name);
	if (it == this->hosts_.end()) {
		this->logger().stream(logger::level::sec_info)
			<< "Host manager: Requested access to nonexistent host: \"" << name
			<< "\", port: " << port << '.';
		
		throw server::host_not_found(name);
	}
	
	auto host_ptr = it->second;
	
	if (!host_ptr->port_allowed(port)) {
		this->logger().stream(logger::level::sec_warning)
			<< "Host manager: Requested access to non-allowed port: host: \"" << name
			<< "\", port: " << port << '.';
		
		throw server::host_not_found(name);
	}
	
	return host_ptr;
}
