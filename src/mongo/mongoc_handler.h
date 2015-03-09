// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef MONGOC_HANDLER_H
#define MONGOC_HANDLER_H

#include <mutex>
#include <mongoc.h>

namespace mongo {

class mongoc_handler
{
public:
	mongoc_handler();
	~mongoc_handler();
	
	static bool was_initialized();
private:
	static std::mutex init_mutex_;
	static bool was_init_;
};

};	// namespace mongo

#endif // MONGOC_HANDLER_H
