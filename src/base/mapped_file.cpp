// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <base/mapped_file.h>

#include <base/mapped_file_exceptions.h>


base::mapped_file::mapped_file(const boost::filesystem::path &path,
							   boost::interprocess::mode_t mode,
							   boost::interprocess::map_options_t map_options):
	path_(path)
{
	using namespace boost::filesystem;
	using namespace boost::interprocess;
	
	if (!exists(this->path())) throw base::path_not_found(this->path().string());
	
	if (is_directory(this->path())) throw base::path_is_directory(this->path().string());
	
	if (!is_empty(this->path())) {
		this->file_mapping_  = std::move(file_mapping(this->path().c_str(), mode));
		this->mapped_region_ = std::move(mapped_region(this->file_mapping_, mode,
													   0, 0, nullptr, map_options));
	}
}
