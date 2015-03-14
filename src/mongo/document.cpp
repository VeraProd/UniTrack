// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include "document.h"

mongo::document::document(bson_t *new_bson, deleter_t d) noexcept:
	document_(new_bson, std::move(d))
{}

mongo::document::document() noexcept:
	document_(bson_new(), bson_destroy)
{}

mongo::document::document(const document &other) noexcept:
	document_(bson_copy(other.raw()), bson_destroy)
{}

mongo::document::document(document &&other) noexcept:
	document_(std::move(other.document_))
{}


mongo::document &
mongo::document::operator=(const document &other) noexcept
{
	return *this = document(other);
}

mongo::document &
mongo::document::operator=(document &&other) noexcept
{
	this->document_ = std::move(other.document_);
	return *this;
}


bool
mongo::document::operator==(const document &other) const noexcept
{ return bson_equal(this->raw(), other.raw()); }


bool
mongo::document::append_utf8(const std::string &key, const std::string &value) noexcept
{ return bson_append_utf8(this->raw(), key.c_str(), (int)key.size(), value.c_str(), (int)value.size()); }

bool
mongo::document::append_int32(const std::string &key, int32_t value) noexcept
{ return bson_append_int32(this->raw(), key.c_str(), (int)key.size(), value); }

bool
mongo::document::append_int64(const std::string &key, int64_t value) noexcept
{ return bson_append_int64(this->raw(), key.c_str(), (int)key.size(), value); }

bool
mongo::document::append_bool(const std::string &key, bool value) noexcept
{ return bson_append_bool(this->raw(), key.c_str(), (int)key.size(), value); }

bool
mongo::document::append_oid(const std::string &key, const bson_oid_t *value) noexcept
{ return bson_append_oid(this->raw(), key.c_str(), (int)key.size(), value); }


bool
mongo::document::append_null(const std::string &key) noexcept
{ return bson_append_null(this->raw(), key.c_str(), (int)key.size()); }


mongo::document::append_class &
mongo::document::append_class::operator()(const std::string &key, const std::string &value)
{
	if (!this->doc_->append_utf8(key, value))
		throw append_error(this->operation_count_);
	++this->operation_count_;
	return *this;
}

mongo::document::append_class &
mongo::document::append_class::operator()(const std::string &key, int32_t value)
{
	if (!this->doc_->append_int32(key, value))
		throw append_error(this->operation_count_);
	++this->operation_count_;
	return *this;
}

mongo::document::append_class &
mongo::document::append_class::operator()(const std::string &key, int64_t value)
{
	if (!this->doc_->append_int64(key, value))
		throw append_error(this->operation_count_);
	++this->operation_count_;
	return *this;
}

mongo::document::append_class &
mongo::document::append_class::operator()(const std::string &key, bool value)
{
	if (!this->doc_->append_bool(key, value))
		throw append_error(this->operation_count_);
	++this->operation_count_;
	return *this;
}

mongo::document::append_class &
mongo::document::append_class::operator()(const std::string &key, const bson_oid_t *value)
{
	if (!this->doc_->append_oid(key, value))
		throw append_error(this->operation_count_);
	++this->operation_count_;
	return *this;
}

mongo::document::append_class &
mongo::document::append_class::operator()(const std::string &key)
{
	if (!this->doc_->append_null(key))
		throw append_error(this->operation_count_);
	++this->operation_count_;
	return *this;
}
