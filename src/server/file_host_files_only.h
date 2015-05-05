// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_FILE_HOST_FILES_ONLY_H
#define SERVER_FILE_HOST_FILES_ONLY_H

#include <string>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <server/types.h>


namespace server {


class files_only
{
public:
	class cache
	{
	public:
		// Data
		boost::interprocess::file_mapping	file_mapping;
		boost::interprocess::mapped_region	mapped_region;
	};
	
	
	std::pair<server::send_buffers_t, server::send_buffers_t>
	operator()(const std::string &path,
			   server::file_host_cache<files_only>::raw_ptr_t cache_ptr);
};	// class files_only


};	// namespace server


#endif // SERVER_FILE_HOST_FILES_ONLY_H
