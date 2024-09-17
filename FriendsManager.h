#pragma once
#ifndef __FRIENDSMANAGER_H
#define __FRIENDSMANAGER_H


ref class Contact {
public:
	System::String^ name;
	System::String^ sipAddress;
	bool isOnline;
};

ref class FriendsManager
{
private:
	System::Collections::Generic::List<Contact^>^ friendsList;
public:
	FriendsManager();
	void addFriend(System::String^ name, System::String^ sipAddress);
	void removeFriend(const System::String^ sipAddress);
	System::Collections::Generic::List<Contact^>^ getFriendsList();
	System::String^ getCurrentFriendSIP(System::String^ contactInfo);
};

#endif // !__FRIENDSMANAGER_H