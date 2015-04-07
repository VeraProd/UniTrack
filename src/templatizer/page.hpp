// Author: Dmitry Kukovinets (d1021976@gmail.com)


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
