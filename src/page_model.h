// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef PAGE_MODEL_H
#define PAGE_MODEL_H

#include <string>
#include <unordered_map>

#include <logger/logger.h>
#include <templatizer/model.h>


class page_model:
	public templatizer::model,
	public std::unordered_map<std::string, std::string>
{
public:
	page_model(logger::logger &logger);
	
	
	virtual const std::string & variable(const std::string &var_name) const override;
private:
	static const std::string unknown_var_;
	
	logger::logger &logger_;
};	// class model


#endif // PAGE_MODEL_H
