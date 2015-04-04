// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <mongo/oid.h>

mongo::oid::oid(bson_context_t *context) noexcept
{ bson_oid_init(&this->oid_, context); }

mongo::oid::oid(const mongo::oid &other) noexcept
{ *this = other; }

mongo::oid &
mongo::oid::operator=(const mongo::oid &other) noexcept
{
	bson_oid_copy(other.raw(), &this->oid_);
	return *this;
}


bool
mongo::oid::operator==(const mongo::oid &other) noexcept
{ return bson_oid_equal(this->raw(), other.raw()); }
