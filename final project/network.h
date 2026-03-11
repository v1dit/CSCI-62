#ifndef NETWORK_H
#define NETWORK_H

#include "post.h"
#include "user.h"

#include <vector>
#include <string>
#include <set>
#include <fstream>
#include <algorithm>


class Network {
public:
    // pre: none
    // post: initializes an empty Network with no users.
    Network();

    // pre: id is positive
    // post: takes in an ID and returns a pointer to the corresponding User. If there is no user with that id, return a nullptr.
    User* getUser(int id) const;

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

    // pre: from and to are the user ids that represent start and end of the path
    // post: returns a vector of user ids representing the shortest path
    std::vector<int> shortestPath(int from, int to);

    // pre: from and to are valid user ids
    // post: returns a vector of all users within the inputed distance of the source user
    std::vector<int> distanceUser(int from, int& to, int distance);

    // pre: who is a valid user id, score is number above 1
    // post: returns a vector of user ids representing suggested friends for the user
    std::vector<int> suggestFriends(int who, int& score);

    //pre: non
    // post: returns a vector of user ids, whcih represents a group
    std::vector<std::vector<int>> groups();

    // pre: post is a valid Post pointer
    // post: adds the Post to the user's list of posts
    void addPost(Post* post);

    // pre: id is a valid user id
    // post: returns a vector of all posts for the user
    std::vector<Post*> getPosts(int id) const;

    // pre: postId is a valid post id
    // post: returns a pointer to the Post with the given id, or nullptr if not found
    Post* getPost(int postId) const;

    // pre: postId is a valid post id
    // post: deletes the Post with the given id, if it exists
    bool deletePost(int postId);

    // pre: name1 and name2 are valid user names
    // post: removes the connection between the two users, if it exists
    void removeConnection(std::string name1, std::string name2);

    // pre: id1 and id2 are valid user ids
    // post: returns the number of mutual friends between the two users
    int getMutualFriendCount(int id1, int id2);

    // pre: fname is a valid filename
    // post: reads posts from a file and adds them to the user's list of posts
    int readPosts(const char* fname);

    // pre: fname is a valid filename
    // post: writes all posts to the specified file
    int writePosts(const char* fname);

    // pre: post is a valid Post pointer
    // post: returns a string representation of the Post
    std::string postDisplayString(Post* post) const;

    // pre: profileId is a valid user id
    // post: returns a string representation of the user's posts
    std::string getPostsString(int profileId, int howMany) const;

   
private:

    std::vector<User*> users_;
    std::vector<std::vector<Post*>> posts_;
    int nextMessagesId_;
};

#endif
