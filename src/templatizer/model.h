// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef TEMPLATIZER_MODEL_H
#define TEMPLATIZER_MODEL_H

#include <string>


namespace templatizer {


class model
{
public:
	virtual ~model() = 0;
	
	
	// Returns value, that variable var_name contains.
	// NOTE: this value must be correct during all sending response to client,
	// so model must cache it.
	virtual const std::string & variable(const std::string &var_name) const = 0;
};	// class model


};	// namespace templatizer


#endif // TEMPLATIZER_MODEL_H
