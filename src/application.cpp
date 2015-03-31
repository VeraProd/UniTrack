// Author: Dmitry Kukovinets (d1021976@gmail.com)
#include <iostream>
#include "application.h"

#include "userprofile.h"

Wt::WApplication *
Application::create(const Wt::WEnvironment &env)
{
	std::cerr << "HERE!!!" << std::endl;
	auto userProfile = new UserProfile;
	std::cerr << "HERE!!!" << std::endl;
	
	auto application = new Wt::WApplication(env);
	application->root()->addWidget(userProfile);
	
	return application;
}
