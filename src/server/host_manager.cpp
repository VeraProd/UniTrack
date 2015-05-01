// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <server/host_manager.h>

#include <new>

#include <server/host_exceptions.h>


server::host_manager::host_manager(logger::logger &logger):
	logger_(logger)
{}


// Adds new host with specifed logger or this host_manager's logger.
// Returns true, if host successfully addes, or false, if host with the
// same name is already managed by this manager (or allocation error occured).
bool
server::host_manager::add_host(logger::logger &logger,
							   const server::host_parameters &parameters) noexcept
{
	try {
		auto new_host_ptr = std::make_shared<server::host>(logger, parameters);
		
		auto p = this->hosts_.emplace(&new_host_ptr->name(), new_host_ptr);
		if (p.second) {
			this->logger_.stream(logger::level::info)
				<< "Host manager: Host " << new_host_ptr->name()
				<< " added.";
		} else {
			this->logger_.stream(logger::level::error)
				<< "Host manager: Host " << new_host_ptr->name()
				<< " not added: Host already exist.";
		}
		
		return p.second;
	} catch (const std::bad_alloc &e) {
		this->logger_.stream(logger::level::error)
			<< "Host manager: Host " << parameters.name
			<< " not added: " << e.what() << '.';
	}
	return false;
}


bool
server::host_manager::add_host(const server::host_parameters &parameters) noexcept
{
	return this->add_host(this->logger_, parameters);
}


// Finds the host with specified name.
// Returns std::shared_ptr to it or throws server::host_not_found exception.
server::host_ptr_t
server::host_manager::host(const std::string &name, unsigned int port)
{
	auto it = this->hosts_.find(&name);
	if (it == this->hosts_.end()) {
		this->logger_.stream(logger::level::sec_info)
			<< "Host manager: Requested access to nonexistent host: \"" << name
			<< "\", port: " << port << '.';
		
		throw server::host_not_found(name);
	}
	
	auto host_ptr = it->second;
	
	if (!host_ptr->port_allowed(port)) {
		this->logger_.stream(logger::level::sec_warning)
			<< "Host manager: Requested access to non-allowed port: host: \"" << name
			<< "\", port: " << port << '.';
		
		throw server::host_not_found(name);
	}
	
	return host_ptr;
}
