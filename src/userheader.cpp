// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include "userheader.h"

#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WImage>
#include <Wt/WLink>
#include <iostream>

UserHeader::UserHeader(Wt::WContainerWidget *parent):
	Wt::WContainerWidget(parent)
{
	std::cerr << "HERE, inside UserHeader constructor!" << std::endl;
	// this->userAvatar_ = new Wt::WImage(Wt::WLink("images/brother.jpg"), this);
	// this->userAvatar_->resize(200, 200);
	
	// auto mainLayout = new Wt::WHBoxLayout(this);
	// mainLayout->addWidget(this->userAvatar_);
	
	// this->setLayout(mainLayout);
}
