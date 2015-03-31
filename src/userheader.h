// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef USERHEADER_H
#define USERHEADER_H

#include <Wt/WContainerWidget>

namespace Wt {
	class WImage;
};

class UserHeader: public Wt::WContainerWidget
{
public:
	UserHeader(Wt::WContainerWidget *parent = nullptr);
private:
	Wt::WImage *userAvatar_;
};

#endif // USERHEADER_H
