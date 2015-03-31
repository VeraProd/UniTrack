// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef APPLICATION_H
#define APPLICATION_H

#include <Wt/WApplication>
#include <Wt/WEnvironment>

class Application: public Wt::WApplication
{
public:
	static Wt::WApplication * create(const Wt::WEnvironment &env);
};

#endif // APPLICATION_H
