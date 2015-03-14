// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <string>
#include <functional>
#include <memory>
#include <stdexcept>
#include <bson.h>

namespace mongo {

class document
{
public:
	typedef std::function<void (bson_t *)> deleter_t;
	
	
	class append_class
	{
	public:
		class append_error: public std::runtime_error
		{
		public:
			append_error(size_t operation_count):
				std::runtime_error("Can't append data to bson."),
				operation_count_(operation_count)
			{}
			
			size_t operation_count() const
			{ return this->operation_count_; }
		private:
			size_t operation_count_;
		};
		
		
		inline append_class(document *doc):
			doc_(doc), operation_count_(0)
		{}
		
		append_class & operator()(const std::string &key, const std::string &value);
		append_class & operator()(const std::string &key, int32_t value);
		append_class & operator()(const std::string &key, int64_t value);
		append_class & operator()(const std::string &key, bool value);
		append_class & operator()(const std::string &key, const bson_oid_t *value);
		
		append_class & operator()(const std::string &key);
	private:
		document *doc_;
		size_t operation_count_;
	};	// append_class
	
	
	document(bson_t *new_doc, deleter_t d) noexcept;
	document() noexcept;
	
	document(const document &other) noexcept;
	document(document &&other) noexcept;
	
	document & operator=(const document &other) noexcept;
	document & operator=(document &&other) noexcept;
	
	
	inline bson_t * raw() const noexcept
	{ return this->document_.get(); }
	
	
	bool operator==(const document &other) const noexcept;
	
	
	inline append_class append()	// Allows to append anything
	{ return append_class(this); }
	
	// Allows to append manually
	bool append_utf8(const std::string &key, const std::string &value) noexcept;
	bool append_int32(const std::string &key, int32_t value) noexcept;
	bool append_int64(const std::string &key, int64_t value) noexcept;
	bool append_bool(const std::string &key, bool value) noexcept;
	bool append_oid(const std::string &key, const bson_oid_t *value) noexcept;
	
	bool append_null(const std::string &key) noexcept;
private:
	std::unique_ptr<bson_t, deleter_t> document_;
};	// document

};	// namespace mongo

#endif // DOCUMENT_H
