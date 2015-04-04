// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef COLLECTION_H
#define COLLECTION_H

#include <string>
#include <memory>
#include <mongoc.h>

#include <mongo/document.h>

namespace mongo {

class collection
{
public:
	collection(mongoc_collection_t *raw_collection) noexcept;
	collection(const collection &other) = delete;
	collection(collection &&other) noexcept;
	
	collection & operator=(const collection &other) = delete;
	collection & operator=(collection &&other) noexcept;
	
	inline mongoc_collection_t * raw() const noexcept
	{ return this->collection_.get(); }
	
	std::string name() const noexcept;
	
	bool drop(bson_error_t *error = nullptr) noexcept;
	bool drop_index(const std::string &index_name, bson_error_t *error = nullptr) noexcept;
	
	bool insert(const mongo::document &doc,
				bson_error_t *error = nullptr,
				mongoc_insert_flags_t flags = MONGOC_INSERT_NONE,
				const mongoc_write_concern_t *write_concern = nullptr);
	
	bool erase(const mongo::document &selector,
			   bson_error_t *error = nullptr,
			   mongoc_remove_flags_t flags = MONGOC_REMOVE_NONE,
			   const mongoc_write_concern_t *write_concern = nullptr);
	
	inline bool erase_single(const mongo::document &selector,
							 bson_error_t *error = nullptr,
							 const mongoc_write_concern_t *write_concern = nullptr)
	{ return this->erase(selector, error, MONGOC_REMOVE_SINGLE_REMOVE, write_concern); }
private:
	std::unique_ptr<mongoc_collection_t, void (*)(mongoc_collection_t *)> collection_;
};

};	// namespace mongo

#endif // COLLECTION_H
