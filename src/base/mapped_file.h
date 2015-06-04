// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef BASE_MAPPED_FILE_H
#define BASE_MAPPED_FILE_H

#include <string>

#include <boost/filesystem.hpp>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>


namespace base {


class mapped_file
{
public:
	mapped_file() = default;	// Empty mapping (newer throws)
	
	// Throws exceptions:
	// - see mapped_file_exceptions.h
	// - boost::filesystem_error
	// - boost::interprocess_error
	mapped_file(const boost::filesystem::path &path,
				boost::interprocess::mode_t mode,
				boost::interprocess::map_options_t map_options
					= boost::interprocess::default_map_options);
	
	mapped_file(mapped_file &&other) = default;
	mapped_file & operator=(mapped_file &&other) = default;
	
	mapped_file(const mapped_file &other) = delete;
	mapped_file & operator=(const mapped_file &other) = default;
	
	
	inline const boost::filesystem::path & path() const noexcept;
	
	inline void * data() const noexcept;
	inline size_t size() const noexcept;
	inline boost::interprocess::mode_t mode() const noexcept;
	
	inline void swap(mapped_file &other);
private:
	boost::filesystem::path path_;
	
	boost::interprocess::file_mapping	file_mapping_;
	boost::interprocess::mapped_region	mapped_region_;
};	// class mapped_file


};	// namespace base


#include <base/mapped_file.hpp>

#endif // BASE_MAPPED_FILE_H
