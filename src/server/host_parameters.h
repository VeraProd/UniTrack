// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef HOST_PARAMETERS_H
#define HOST_PARAMETERS_H

#include <string>
#include <vector>

#include <json.hpp>

#include <server/types.h>


namespace server {


// Host parameters. Need for every host.
// This is default parameters for error_host.
struct host_parameters
{
	std::string					name;			// Required
	port_set_t					ports;			// Required
	
	std::vector<std::string>	server_names =	// Optional
		{
			"UniTrack/0.0.1",
			"Tired of reading the headers? Look for the UniTrack project on GitHub!",
			"ZX_Spectrum/1997 (Sinclair_BASIC)",
			"c64/1986 (**** COMMODORE 64 BASIC V2 **** 64K RAM SYSTEM ****)",
			"thttpd/1.02 (Minix 2.0.2 i186)",
			"pear/6.2 (iOS 5.0.1)",
			"segasrv/1.0 (SEGA MEGA DRIVE HTTP SERVER ROM 1.0)",
			"Tea with milk (Full cup, with sugar)"
		};
	
	
	explicit host_parameters() = default;
	explicit host_parameters(const nlohmann::json &config);
};	// struct host_parameters


};	// namespace server


#endif // HOST_PARAMETERS_H
