// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
const boost::filesystem::path &
base::mapped_file::path() const noexcept
{
	return this->path_;
}


inline
void *
base::mapped_file::data() const noexcept
{
	return this->mapped_region_.get_address();
}


inline
size_t
base::mapped_file::size() const noexcept
{
	return this->mapped_region_.get_size();
}


inline
boost::interprocess::mode_t
base::mapped_file::mode() const noexcept
{
	return this->mapped_region_.get_mode();
}


inline
void
base::mapped_file::swap(base::mapped_file &other)
{
	this->mapped_region_.swap(other.mapped_region_);
	this->file_mapping_.swap(other.file_mapping_);
}
