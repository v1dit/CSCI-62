#ifndef USER_H
#define USER_H

#include <string>
#include <set>

class User {
public:
    // pre: none
    // post: creates a user with arbitrary values
    User();

    // pre: none
    // post: creates a User with given id, name, year, zip, and friends
    User(int id, std::string name, int year, int zip, std::set<int> friends);

    // pre: none
    // post: returns the user's id
    int getId() const;

    // pre: none
    // post: returns the user's name
    std::string getName() const;

    // pre: none
    // post: returns the user's birth year
    int getYear() const;

    // pre: none
    // post: returns the user's zip code
    int getZip() const;

    // pre: none
    // post: returns a reference to the set of friend ids
    std::set<int>& getFriends();

    // pre: if id is a positiive integer
    // post: adds id to friends if not already present
    void addFriend(int id);

    // pre: if id is a positive integer
    // post: removes id from friends if present
    void deleteFriend(int id);

private:
    int id_;
    std::string name_;
    int year_;
    int zip_;
    std::set<int> friends_;
};

#endif
