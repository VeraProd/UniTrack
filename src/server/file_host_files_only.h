// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_FILE_HOST_FILES_ONLY_H
#define SERVER_FILE_HOST_FILES_ONLY_H

#include <string>

#include <boost/filesystem/path.hpp>

#include <base/mapped_file.h>

#include <server/types.h>


namespace server {


class files_only
{
public:
	class cache
	{
	public:
		// Data
		base::mapped_file mapped_file;
	};
	
	
	std::pair<server::send_buffers_t, server::send_buffers_t>
	operator()(const boost::filesystem::path &path,
			   server::file_host_cache<files_only>::raw_ptr_t cache_ptr);
};	// class files_only


};	// namespace server


#endif // SERVER_FILE_HOST_FILES_ONLY_H
