// Author: Dmitry Kukovinets (d1021976@gmail.com)
#include <iostream>
#include "userprofile.h"

#include "userheader.h"

UserProfile::UserProfile(Wt::WContainerWidget *parent):
	Wt::WContainerWidget(parent)
{
	std::cerr << "HERE!!!" << std::endl;
	this->header_ = new UserHeader;
	this->addWidget(this->header_);
	std::cerr << "HERE 2!!!" << std::endl;
}
