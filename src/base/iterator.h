// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef BASE_ITERATOR_H
#define BASE_ITERATOR_H

#include <memory>


namespace base {


// Back extractor
template<class Container>
class back_extract_iterator
{
public:
	typedef Container container_type;
	
	
	explicit back_extract_iterator(Container &container) noexcept:
		container_ptr_(std::addressof(container))
	{}
	
	
	typename Container::value_type & operator*() noexcept
	{ return this->container_ptr_->back(); }
	
	typename Container::value_type * operator->() noexcept
	{ return std::addressof(this->container_ptr_->back()); }
	
	
	back_extract_iterator<Container> & operator++() noexcept
	{ return *this; }
	
	back_extract_iterator<Container> operator++(int) noexcept
	{ return *this; }
protected:
	Container* container_ptr_;
};	// class back_extract_iterator


template<class Container>
inline back_extract_iterator<Container> back_extractor(Container &container) noexcept
{
	return back_extract_iterator<Container>(container);
}


}	// namespace base


#endif // BASE_ITERATOR_H
