// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef BASE_FUNCTIONAL_H
#define BASE_FUNCTIONAL_H


namespace base {


template<class Container>
class back_inserter_functor
{
public:
	typedef Container container_type;
	
	
	inline back_inserter_functor(Container &container);
	
	back_inserter_functor(const back_inserter_functor &other) = default;
	back_inserter_functor(back_inserter_functor &&other) = default;
	
	back_inserter_functor & operator=(const back_inserter_functor &other) = default;
	back_inserter_functor & operator=(back_inserter_functor &&other) = default;
	
	
	const typename Container::value_type & operator()(const typename Container::value_type &data);
	const typename Container::value_type & operator()(typename Container::value_type &&data);
	
	template<class ...Args>
	const typename Container::value_type & operator()(Args &&...args);
private:
	Container *container_ptr_;
};	// class back_inserter_functor


template<class Container>
inline back_inserter_functor<Container> make_back_inserter_functor(Container &container);


};	// namespace base


#include <base/functional.hpp>

#endif // BASE_FUNCTIONAL_H
