// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef USERPROFILE_H
#define USERPROFILE_H

#include <Wt/WContainerWidget>

class UserHeader;

class UserProfile: public Wt::WContainerWidget
{
public:
	UserProfile(Wt::WContainerWidget *parent = nullptr);
private:
	UserHeader *header_;
};

#endif // USERPROFILE_H
