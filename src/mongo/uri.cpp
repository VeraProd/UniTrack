// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include "uri.h"

mongo::uri::uri(const std::string &uri):
	raw_uri_(mongoc_uri_new(uri.c_str()),
			 mongoc_uri_destroy)
{}
