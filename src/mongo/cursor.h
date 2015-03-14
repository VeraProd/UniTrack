// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef CURSOR_H
#define CURSOR_H

#include <memory>
#include <mongoc.h>

namespace mongo {

class cursor
{
public:
	cursor(mongoc_cursor_t *raw_cursor) noexcept;
	cursor(const cursor &other) noexcept;
	cursor(cursor &&other) noexcept;
	
	cursor & operator=(const cursor &other) noexcept;
	cursor & operator=(cursor &&other) noexcept;
	
	inline mongoc_cursor_t * raw() const noexcept
	{ return this->cursor_.get(); }
	
	const bson_t * current() const noexcept;
	bool more() const noexcept;
	bool next(const bson_t * &bson) const noexcept;
	
	bool alive() const noexcept;
	bool error(bson_error_t *error = nullptr) noexcept;
private:
	std::unique_ptr<mongoc_cursor_t, void (*)(mongoc_cursor_t *)> cursor_;
};

};	// namespace mongo

#endif // CURSOR_H
