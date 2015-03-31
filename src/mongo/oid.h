// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef OID_H
#define OID_H

#include <bson.h>

namespace mongo {

class oid
{
public:
	oid(bson_context_t *context = nullptr) noexcept;
	oid(const oid &other) noexcept;
	
	oid & operator=(const oid &other) noexcept;
	
	inline const bson_oid_t * raw() const noexcept
	{ return &this->oid_; }
	
	bool operator==(const oid &other) noexcept;
private:
	bson_oid_t oid_;
};	// oid

};	// namespace mongo

#endif // OID_H
