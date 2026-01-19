#include "user.h"


// Default constructor
User::User()
    : id_(0), name_(""), year_(0), zip_(0), friends_()
{
}

// constructor
User::User(int id, std::string name, int year, int zip, std::set<int> friends)
    : id_(id), name_(name), year_(year), zip_(zip), friends_(friends)
{
}

//returns id 
int User::getId() const {
    return id_;
}

// returns name 
std::string User::getName() const {
    return name_;
}

// returns year
int User::getYear() const {
    return year_;
}

// returns zip 
int User::getZip() const {
    return zip_;
}

// return friends list 
std::set<int>& User::getFriends() {
    return friends_;
}

// adds friend to list
void User::addFriend(int id) {
    friends_.insert(id);
}

// deletes friend from list
void User::deleteFriend(int id) {
    friends_.erase(id);
}
