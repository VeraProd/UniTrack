// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef MONGO_URI_H
#define MONGO_URI_H

#include <string>
#include <memory>
#include <stdexcept>
#include <mongoc.h>

namespace mongo {

class uri
{
public:
	uri(const std::string &uri_string) noexcept;
	uri(const uri &other) noexcept;
	uri(uri &&other) noexcept;
	
	uri & operator=(const uri &other) noexcept;
	uri & operator=(uri &&other) noexcept;
	
	inline mongoc_uri_t * raw() const noexcept
	{ return this->uri_.get(); }
	
	inline bool ok() const noexcept
	{ return this->uri_ != nullptr; }
private:
	std::unique_ptr<mongoc_uri_t, void (*)(mongoc_uri_t *)> uri_;
};

};

#endif // MONGO_URI_H
