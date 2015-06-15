// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef BASE_CACHE_H
#define BASE_CACHE_H

#include <string>
#include <vector>
#include <deque>

#include <base/iterator.h>
#include <base/functional.h>


namespace base {


// Strings cache
typedef std::deque<std::string>							strings_cache_t;
typedef std::back_insert_iterator<strings_cache_t>		strings_cache_insert_iterator_t;
typedef base::back_extract_iterator<strings_cache_t>	strings_cache_extract_iterator_t;
typedef base::back_inserter_functor<strings_cache_t>	strings_cache_insert_functor_t;


};	// namespace base


#endif // BASE_CACHE_H
