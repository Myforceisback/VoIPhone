#include "MyBuddy.h"

void MyBuddy::onBuddyState()
{
	pj::BuddyInfo bi = getInfo();
	std::cout << "Buddy " << bi.uri << " is " << bi.presStatus.statusText << std::endl;
}
