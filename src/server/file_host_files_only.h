// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef SERVER_FILE_HOST_FILES_ONLY_H
#define SERVER_FILE_HOST_FILES_ONLY_H

#include <string>

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
	
	
	template<class FileHost>
	std::pair<server::send_buffers_t, server::send_buffers_t>
	operator()(const FileHost &host,
			   server::file_host_cache<files_only> &cache);
};	// class files_only


};	// namespace server


#include <server/file_host_files_only.hpp>

#endif // SERVER_FILE_HOST_FILES_ONLY_H
