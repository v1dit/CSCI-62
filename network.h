#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <string>
#include "user.h"

class Network {
public:
    // pre: none
    // post: initializes an empty Network with no users.
    Network();

    // pre: id is positive
    // post: takes in an ID and returns a pointer to the corresponding User. If there is no user with that id, return a nullptr.
    User* getUser(int id);

    // pre: accept a pointer to a User object.
    // post: add a User to the Network database.
    void addUser(User* user);

    // pre: takes two strings each in the [first-name] [lastname] format
    // post: adds friend connection if both users exist returns -1 if either user is invalid, 0 otherwise
    int addConnection(std::string s1, std::string s2);

    // pre: takes two strings each in the [first-name] [lastname] format
    // post: delete the friend connection if it exists. Return -1 if either user is invalid and 0 otherwise.
    int deleteConnection(std::string s1, std::string s2);

    // pre: takes a string in the [firstname] [lastname] format
    // post: returns the id for that user, or -1 if no user exists.
    int getId(std::string name);

    // pre: none
    // post: returns the number of Users in the Network.
    int numUsers();

    // pre: valid filename
    // post: initializes all of the network’s information from a file
    void readUsers(const char* fname);

    // pre: valid filename
    // post: writes all of the network’s information to a file
    void writeUsers(const char* fname);

private:
    std::vector<User*> users_;
};

#endif
