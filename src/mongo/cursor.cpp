// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <mongo/cursor.h>

mongo::cursor::cursor(mongoc_cursor_t *raw_cursor) noexcept:
	cursor_(raw_cursor, mongoc_cursor_destroy)
{}

mongo::cursor::cursor(const cursor &other) noexcept:
	cursor_(mongoc_cursor_clone(other.cursor_.get()), mongoc_cursor_destroy)
{}

mongo::cursor::cursor(cursor &&other) noexcept:
	cursor_(std::move(other.cursor_))
{}


mongo::cursor &
mongo::cursor::operator=(const cursor &other) noexcept
{
	return *this = std::move(cursor(other));
}

mongo::cursor &
mongo::cursor::operator=(cursor &&other) noexcept
{
	this->cursor_ = std::move(other.cursor_);
	return *this;
}


const bson_t *
mongo::cursor::current() const noexcept
{ return mongoc_cursor_current(this->raw()); }

bool
mongo::cursor::more() const noexcept
{ return mongoc_cursor_more(this->raw()); }

bool
mongo::cursor::next(const bson_t * &bson) const noexcept
{ return mongoc_cursor_next(this->raw(), &bson); }


bool
mongo::cursor::alive() const noexcept
{ return mongoc_cursor_is_alive(this->raw()); }

bool
mongo::cursor::error(bson_error_t *error) noexcept
{ return mongoc_cursor_error(this->raw(), error); }
