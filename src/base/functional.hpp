// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <memory>


template<class Container>
inline
base::back_inserter_functor<Container>::back_inserter_functor(Container &container):
	container_ptr_(std::addressof(container))
{}


template<class Container>
const typename Container::value_type &
base::back_inserter_functor<Container>::operator()(const typename Container::value_type &data)
{
	this->container_ptr_->push_back(data);
	return this->container_ptr_->back();
}


template<class Container>
const typename Container::value_type &
base::back_inserter_functor<Container>::operator()(typename Container::value_type &&data)
{
	this->container_ptr_->push_back(std::move(data));
	return this->container_ptr_->back();
}


template<class Container>
template<class ...Args>
const typename Container::value_type &
base::back_inserter_functor<Container>::operator()(Args &&...args)
{
	this->container_ptr_->emplace_back(std::forward<Args &&>(args)...);
	return this->container_ptr_->back();
}


template<class Container>
inline
base::back_inserter_functor<Container>
base::make_back_inserter_functor(Container &container)
{
	return base::back_inserter_functor<Container>(container);
}
