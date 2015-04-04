// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_MODEL_H
#define TEMPLATIZER_MODEL_H

#include <string>


namespace templatizer {

class model
{
public:
	model();
	virtual ~model() = 0;
	
	virtual const std::string & at(const std::string &var_name) const = 0;
};	// class model

};	// namespace templatizer

#endif // TEMPLATIZER_MODEL_H
