// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef MONGO_DOCUMENT_H
#define MONGO_DOCUMENT_H

#include <string>
#include <functional>
#include <memory>
#include <stdexcept>
#include <bson.h>

#include <mongo/oid.h>

namespace mongo {

class document
{
public:
	class append_error: public std::runtime_error
	{
	public:
		inline append_error(size_t operation_count):
			std::runtime_error("Can't append data to bson."),
			operation_count_(operation_count)
		{}
		
		inline size_t operation_count() const
		{ return this->operation_count_; }
	private:
		size_t operation_count_;
	};
	
	class concat_error: public std::runtime_error
	{
	public:
		inline concat_error():
			std::runtime_error("Can't concatenate bsons.")
		{}
	};
	
	class json_error: public std::runtime_error
	{
	public:
		inline json_error(const std::string &error_message):
			std::runtime_error(error_message)
		{}
	};
	
	
	class append_class
	{
	public:
		inline append_class(document *doc):
			doc_(doc), operation_count_(0)
		{}
		
		inline size_t operation_count() const noexcept
		{ return this->operation_count_; }
		
		append_class & operator()(const std::string &key, const std::string &value);
		append_class & operator()(const std::string &key, const char *value);
		append_class & operator()(const std::string &key, int32_t value);
		append_class & operator()(const std::string &key, int64_t value);
		append_class & operator()(const std::string &key, double value);
		append_class & operator()(const std::string &key, bool value);
		append_class & operator()(const std::string &key, const mongo::oid &value);
		
		append_class & operator()(const std::string &key);
		
		append_class & date_time(const std::string &key, int64_t value);
		append_class & now_utc(const std::string &key);
	private:
		document *doc_;
		size_t operation_count_;
	};	// append_class
	
	
	typedef std::function<void (bson_t *)> deleter_t;
	
	document(bson_t *new_doc, deleter_t d) noexcept;
	document(const std::string &json_string, bson_error_t *error = nullptr) noexcept;
	document(size_t size) noexcept;	// Sized version: preallocates size bytes-buffer
	document() noexcept;
	
	document(const document &other) noexcept;
	document(document &&other) noexcept;
	
	document & operator=(const document &other) noexcept;
	document & operator=(document &&other) noexcept;
	
	
	inline bson_t * raw() const noexcept
	{ return this->document_.get(); }
	
	
	bool operator==(const document &other) const noexcept;
	document & operator+=(const document &other);
	
	
	uint32_t size() const noexcept;
	bool has_field(const std::string &key) const noexcept;
	bool concat(const document &other) noexcept;
	
	std::string json() const noexcept;
	void json(const std::string &json_string) noexcept;
	
	// Append
	inline append_class append()	// Allows to append anything
	{ return append_class(this); }
	
	// Allows to append manually
	bool append_utf8(const std::string &key, const std::string &value) noexcept;
	bool append_utf8(const std::string &key, const char *value) noexcept;
	bool append_int32(const std::string &key, int32_t value) noexcept;
	bool append_int64(const std::string &key, int64_t value) noexcept;
	bool append_double(const std::string &key, double value) noexcept;
	bool append_bool(const std::string &key, bool value) noexcept;
	bool append_oid(const std::string &key, const mongo::oid &value) noexcept;
	
	bool append_date_time(const std::string &key, int64_t value) noexcept;
	bool append_now_utc(const std::string &key) noexcept;
	
	bool append_null(const std::string &key) noexcept;
private:
	std::unique_ptr<bson_t, deleter_t> document_;
};	// document

};	// namespace mongo

#endif // MONGO_DOCUMENT_H
