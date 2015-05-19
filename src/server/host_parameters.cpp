// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <host_parameters.h>


server::host_parameters::host_parameters(const nlohmann::json &config):
	name(config.at("name").get<decltype(this->name)>()),
	ports(config.at("ports").get<decltype(this->ports)>())
{
	try {
		this->server_names = config.at("server_names").get<decltype(this->server_names)>();
	} catch (const std::out_of_range &) {}
}
