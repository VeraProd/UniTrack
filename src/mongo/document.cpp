// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <mongo/document.h>

#include <cstring>

// document::append_class
mongo::document::append_class &
mongo::document::append_class::operator()(const std::string &key, const std::string &value)
{
	if (!this->doc_->append_utf8(key, value))
		throw append_error(this->operation_count_);
	++this->operation_count_;
	return *this;
}

mongo::document::append_class &
mongo::document::append_class::operator()(const std::string &key, const char *value)
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
mongo::document::append_class::operator()(const std::string &key, double value)
{
	if (!this->doc_->append_double(key, value))
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
mongo::document::append_class::operator()(const std::string &key, const mongo::oid &value)
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


mongo::document::append_class &
mongo::document::append_class::date_time(const std::string &key, int64_t value)
{
	if (!this->doc_->append_date_time(key, value))
		throw append_error(this->operation_count_);
	++this->operation_count_;
	return *this;
}

mongo::document::append_class &
mongo::document::append_class::now_utc(const std::string &key)
{
	if (!this->doc_->append_now_utc(key))
		throw append_error(this->operation_count_);
	++this->operation_count_;
	return *this;
}



// document
mongo::document::document(bson_t *new_bson, deleter_t d) noexcept:
	document_(new_bson, std::move(d))
{}

mongo::document::document(const std::string &json_string, bson_error_t *error) noexcept:
	document_(bson_new_from_json(reinterpret_cast<const uint8_t *>(json_string.c_str()),
								 (ssize_t)json_string.size(), error),
			  bson_destroy)
{}

mongo::document::document(size_t size) noexcept:
	document_(bson_sized_new(size), bson_destroy)
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
mongo::document::operator==(const mongo::document &other) const noexcept
{ return bson_equal(this->raw(), other.raw()); }

mongo::document &
mongo::document::operator+=(const mongo::document &other)
{
	if (!this->concat(other))
		throw mongo::document::concat_error();
	return *this;
}


// Other functions
uint32_t
mongo::document::size() const noexcept
{ return bson_count_keys(this->raw()); }

bool
mongo::document::has_field(const std::string &key) const noexcept
{ return bson_has_field(this->raw(), key.c_str()); }


bool
mongo::document::concat(const mongo::document &other) noexcept
{ return bson_concat(this->raw(), other.raw()); }

std::string
mongo::document::json() const noexcept
{
	size_t len = 0;
	char *raw_str = bson_as_json(this->raw(), &len);
	if (raw_str == nullptr)
		throw mongo::document::json_error("Unknown bson_as_json error.");
	
	std::string res(raw_str, len);
	bson_free(raw_str);
	return res;
}

void
mongo::document::json(const std::string &json_string) noexcept
{
	bson_error_t error;
	if (!bson_init_from_json(this->raw(),
							 json_string.c_str(),
							 (ssize_t)json_string.size(),
							 &error))
		throw json_error(error.message);
}


// Append functions
bool
mongo::document::append_utf8(const std::string &key, const std::string &value) noexcept
{ return bson_append_utf8(this->raw(), key.c_str(), (int)key.size(), value.c_str(), (int)value.size()); }

bool
mongo::document::append_utf8(const std::string &key, const char *value) noexcept
{ return bson_append_utf8(this->raw(), key.c_str(), (int)key.size(), value, (int)std::strlen(value)); }

bool
mongo::document::append_int32(const std::string &key, int32_t value) noexcept
{ return bson_append_int32(this->raw(), key.c_str(), (int)key.size(), value); }

bool
mongo::document::append_int64(const std::string &key, int64_t value) noexcept
{ return bson_append_int64(this->raw(), key.c_str(), (int)key.size(), value); }

bool
mongo::document::append_double(const std::string &key, double value) noexcept
{ return bson_append_double(this->raw(), key.c_str(), (int)key.size(), value); }

bool
mongo::document::append_bool(const std::string &key, bool value) noexcept
{ return bson_append_bool(this->raw(), key.c_str(), (int)key.size(), value); }

bool
mongo::document::append_oid(const std::string &key, const mongo::oid &value) noexcept
{ return bson_append_oid(this->raw(), key.c_str(), (int)key.size(), value.raw()); }


bool
mongo::document::append_date_time(const std::string &key, int64_t value) noexcept
{ return bson_append_date_time(this->raw(), key.c_str(), (int)key.size(), value); }

bool
mongo::document::append_now_utc(const std::string &key) noexcept
{ return bson_append_now_utc(this->raw(), key.c_str(), (int)key.size()); }


bool
mongo::document::append_null(const std::string &key) noexcept
{ return bson_append_null(this->raw(), key.c_str(), (int)key.size()); }
