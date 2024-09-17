#include "FriendsManager.h"

FriendsManager::FriendsManager() {
    friendsList = gcnew System::Collections::Generic::List<Contact^>();
    
}

void FriendsManager::addFriend(System::String^ name, System::String^ sipAddress) {
    Contact^ newFriend = gcnew Contact;
    newFriend->name = name;
    newFriend->sipAddress = sipAddress;
    newFriend->isOnline = false; // По умолчанию считаем, что друг оффлайн
    friendsList->Add(newFriend);
}

void FriendsManager::removeFriend(const System::String^ sipAddress) {
    
}

System::Collections::Generic::List<Contact^>^ FriendsManager::getFriendsList()
{
    return friendsList;
}

System::String^ FriendsManager::getCurrentFriendSIP(System::String^ contactInfo)
{
    //auto temp = friendsList->Find()
    return "dada";
}
