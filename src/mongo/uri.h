// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef URI_H
#define URI_H

#include <string>
#include <memory>
#include <mongoc.h>

namespace mongo {

class uri
{
public:
	uri(const std::string &uri);
	
	inline mongoc_uri_t * raw() const
	{ return this->raw_uri_.get(); }
private:
	std::unique_ptr<mongoc_uri_t, void (*)(mongoc_uri_t *)> raw_uri_;
};

};

#endif // URI_H
