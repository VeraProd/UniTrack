// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATE_PAGE_MODEL_H
#define TEMPLATE_PAGE_MODEL_H

#include <string>


class template_page_model
{
public:
	template_page_model();
	virtual ~template_page_model() = 0;
	
	virtual const std::string & at(const std::string &var_name) const = 0;
};	// class template_page_model

#endif // TEMPLATE_PAGE_MODEL_H
