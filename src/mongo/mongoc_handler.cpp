// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include "mongoc_handler.h"

std::mutex mongo::mongoc_handler::init_mutex_;
bool mongo::mongoc_handler::was_init_ = false;


mongo::mongoc_handler::mongoc_handler()
{
	mongo::mongoc_handler::init_mutex_.lock();
	if (!mongo::mongoc_handler::was_init_) {
		mongoc_init();
		mongo::mongoc_handler::was_init_ = true;
	}
	mongo::mongoc_handler::init_mutex_.unlock();
}


mongo::mongoc_handler::~mongoc_handler()
{
	mongo::mongoc_handler::init_mutex_.lock();
	if (mongo::mongoc_handler::was_init_) {
		mongoc_cleanup();
		mongo::mongoc_handler::was_init_ = false;
	}
	mongo::mongoc_handler::init_mutex_.unlock();
}


bool
mongo::mongoc_handler::was_initialized()
{
	mongo::mongoc_handler::init_mutex_.lock();
	bool res = mongo::mongoc_handler::was_init_;
	mongo::mongoc_handler::init_mutex_.unlock();
	return res;
}
