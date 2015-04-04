// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <mongo/collection.h>

mongo::collection::collection(mongoc_collection_t *raw_collection) noexcept:
	collection_(raw_collection, mongoc_collection_destroy)
{}

mongo::collection::collection(collection &&other) noexcept:
	collection_(std::move(other.collection_))
{}


mongo::collection &
mongo::collection::operator=(collection &&other) noexcept
{
	this->collection_ = std::move(other.collection_);
	return *this;
}


std::string
mongo::collection::name() const noexcept
{ return std::string(mongoc_collection_get_name(this->raw())); }


bool
mongo::collection::drop(bson_error_t *error) noexcept
{ return mongoc_collection_drop(this->collection_.get(), error); }

bool
mongo::collection::drop_index(const std::string &index_name, bson_error_t *error) noexcept
{ return mongoc_collection_drop_index(this->collection_.get(), index_name.c_str(), error); }


bool
mongo::collection::insert(const mongo::document &doc,
						  bson_error_t *error,
						  mongoc_insert_flags_t flags,
						  const mongoc_write_concern_t *write_concern)
{ return mongoc_collection_insert(this->raw(), flags, doc.raw(), write_concern, error); }

bool
mongo::collection::erase(const mongo::document &selector,
						 bson_error_t *error,
						 mongoc_remove_flags_t flags,
						 const mongoc_write_concern_t *write_concern)
{ return mongoc_collection_remove(this->raw(), flags, selector.raw(), write_concern, error); }
