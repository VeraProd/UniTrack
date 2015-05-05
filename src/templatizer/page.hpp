// Author: Dmitry Kukovinets (d1021976@gmail.com)


// State
inline
enum templatizer::page::state
templatizer::page::state() const noexcept
{
	return this->state_;
}


inline
bool
templatizer::page::good() const noexcept
{
	return this->state_ == templatizer::page::state::ok;
}


inline
bool
templatizer::page::bad() const noexcept
{
	return !this->good();
}


inline
void
templatizer::page::set_state(enum templatizer::page::state new_state) noexcept
{
	this->state_ = new_state;
}


struct templatizer::page::page_printer
{
	const templatizer::page &page;
	const templatizer::model &model;
};


inline
templatizer::page::page_printer
templatizer::page::operator()(const templatizer::model &model) const
{
	return page_printer{ .page = *this, .model = model };
}


inline
std::ostream &
operator<<(std::ostream &stream, const templatizer::page::page_printer &printer)
{
	printer.page.generate(stream, printer.model);
	return stream;
}
