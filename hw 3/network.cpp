#include "network.h"
#include "post.h"

#include <fstream>
#include <sstream>
#include <set>
#include <queue>
#include <stack>
#include <algorithm>


Network::Network(){
    nextMessagesId_ = 0;
}


// returns a pointer to the user with the given id or nullptr if not found
// id needs to be positive and greater than users_.size
User* Network::getUser(int id) const {
    if (id < 0 || id >= users_.size()) {
        return nullptr;
    }
    return users_[id];
}

// adds a user to the network
void Network::addUser(User* user) {
    users_.push_back(user);
    // ensure posts_ has an entry for each user
    if (posts_.size() < users_.size()) {
        posts_.resize(users_.size());
    }
}

// returns the id for a user with the given name or -1 if not found
int Network::getId(std::string name) {
    for (int i = 0; i < users_.size(); i++) {
        if (users_[i]->getName() == name) {
            return i;
        }
    }
    return -1;
}

// adds a connection between two users
int Network::addConnection(std::string s1, std::string s2) {
    int id1 = getId(s1);
    int id2 = getId(s2);

// if either user doesnt exist
    if (id1 == -1 || id2 == -1) {
        return -1;
    }

    users_[id1]->addFriend(id2);
    users_[id2]->addFriend(id1);
    return 0;
}

// deletes a connection between two users
int Network::deleteConnection(std::string s1, std::string s2) {
    int id1 = getId(s1);
    int id2 = getId(s2);

// if either user doesnt exist
    if (id1 == -1 || id2 == -1) {
        return -1;
    }

    users_[id1]->deleteFriend(id2);
    users_[id2]->deleteFriend(id1);
    return 0;
}

// returns number of users
int Network::numUsers() {
    return users_.size();
}

// reads users from a file
void Network::readUsers(const char* fname) {
    std::ifstream infile(fname);
    if (!infile) {
        return;
    }

// read number of users
    int n;
    infile >> n;
    infile.ignore();

// loop per user
    for (int i = 0; i < n; i++) {
        int id;
        infile >> id;
        infile.ignore();

// read the name line
        std::string name;
        std::getline(infile, name);
        name = name.substr(1);  // remove tab

// read the birth year
        int year;
        infile >> year;
        infile.ignore();

// read the zip 
        int zip;
        infile >> zip;
        infile.ignore();

// read friends list
        std::string line;
        std::getline(infile, line);

// make friends into set of ints 
        std::set<int> friends;
        std::stringstream ss(line);
        int fid;
        while (ss >> fid) {
            friends.insert(fid);
        }

// create user and add to network
        User* u = new User(id, name, year, zip, friends);
        users_.push_back(u);
    }
    // ensure posts_ has entries matching loaded users
    if (posts_.size() < users_.size()) {
        posts_.resize(users_.size());
    }
}


// writes users to a file
void Network::writeUsers(const char* fname) {
    std::ofstream outfile(fname);
    if (!outfile) {
        return;
    }

    // the number of users (single number)
    outfile << users_.size() << "\n";

    for (User* u : users_) {
        // id_0
        outfile << u->getId() << "\n";
        // user_name
        outfile << "\t" << u->getName() << "\n";
        // birth year
        outfile << "\t" << u->getYear() << "\n";
        // zip code 
        outfile << "\t" << u->getZip() << "\n";
        // friends list
        outfile << "\t";
        // loop through friends
        for (int fid : u->getFriends()) {
            outfile << fid << " ";
        }
        outfile << "\n";
    }
}

std::vector<int> Network::shortestPath(int from, int to) {
    std::vector<int> path;

    // if the path is not valid
    if (from < 0 || from >= users_.size() || to < 0 || to >= users_.size()) {
        return path;
    }

    // BFS
    std::vector<bool> visited(users_.size(), false);
    std::vector<int> prev(users_.size(), -1);
    std::queue<int> q;

    // intialize
    visited[from] = true;
    q.push(from);

    // loop until no more nodes 
    while (!q.empty()) {
        int curr = q.front();
        q.pop();

        // check if destination is reached
        if (curr == to) {
            for (int x = to; x != -1; x = prev[x]) {
                path.push_back(x);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        // explore neighbors
        for (int neighbor : users_[curr]->getFriends()) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                prev[neighbor] = curr;
                q.push(neighbor);
            }
        }
    }
    
    return path;
    }

std::vector<int> Network::distanceUser(int from, int& to, int distance) {
    std::vector<int> path;

    // invalid starting user
    if (from < 0 || from >= users_.size()) {
        to = -1;
        return path;
    }

    int n = users_.size();

    std::vector<bool> visited(n, false);
    std::vector<int> prev(n, -1);
    std::vector<int> dist(n, -1);
    std::queue<int> q;

    visited[from] = true;
    dist[from] = 0;
    q.push(from);

    // loop until no more nodes 
    while (!q.empty()) {
        int curr = q.front();
        q.pop();

        // found user at exact distance
        if (dist[curr] == distance) {
            to = curr;

            for (int x = curr; x != -1; x = prev[x]) {
                path.push_back(x);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        // explore neighbors
        for (int neighbor : users_[curr]->getFriends()) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                dist[neighbor] = dist[curr] + 1;
                prev[neighbor] = curr;
                q.push(neighbor);
            }
        }
    }

    // no user found at that distance
    to = -1;
    return path;
}

std::vector<int> Network::suggestFriends(int who, int& score) {
    std::vector<int> result;
    score = 0;

    // invalid user
    if (who < 0 || who >= users_.size()) {
        return result;
    }

    const std::set<int>& myFriends = users_[who]->getFriends();

    for (int i = 0; i < users_.size(); i++) {

        // only consider users who are not the same user and not already friends
        if (i != who && myFriends.count(i) == 0) {

            int common = 0;
            const std::set<int>& theirFriends = users_[i]->getFriends();

            // count common friends
            for (int f : myFriends) {
                if (theirFriends.count(f)) {
                    common++;
                }
            }

            // update score
            if (common > score) {
                score = common;
                result.clear();
                if (common > 0) {
                    result.push_back(i);
                }
            }
            else if (common == score && common > 0) {
                result.push_back(i);
            }
        }
    }

    // no valid suggestions
    if (score == 0) {
        result.clear();
    }

    return result;
}

std::vector<std::vector<int>> Network::groups() {

    // vector for all ids
    std::vector<std::vector<int>> allGroups;
    // all users already in a group
    std::vector<bool> visited(users_.size(), false);

    // DFS
    // Explore all friends of each unvisited user
    for (int i = 0; i < users_.size(); i++) {
        if (!visited[i]) {
            std::vector<int> group;
            std::stack<int> s;

            s.push(i);
            visited[i] = true;
            // explore all friends of the current user
            while (!s.empty()) {
                int curr = s.top();
                s.pop();
                group.push_back(curr);

                for (int friendId : users_[curr]->getFriends()) {
                    if (!visited[friendId]) {
                        visited[friendId] = true;
                        s.push(friendId);
                    }
                }
            }
            // add the found group to the list of all groups
            allGroups.push_back(group);
        }
    }
    // all groups found
    return allGroups;
}


void Network::addPost(Post* post) {
    int profileId = post->getProfileId();

    if (profileId >= posts_.size()) {
        posts_.resize(profileId + 1);
    }

    // assign next available message id
    post->setMessageId(nextMessagesId_);
    nextMessagesId_++;

    posts_[profileId].push_back(post);
}



// returns a vector of post pointers 
std::vector<Post*> Network::getPosts(int id) const {
    if (id < 0 || id >= posts_.size()) {
        return {};
    }
    return posts_[id];
}

// compares sorted posts
static bool comparePosts(Post* a, Post* b) {
    return a->getMessageId() < b->getMessageId();
}

// reads posts from a file and adds them to the user's list of posts
int Network::readPosts(const char* fname) {
    std::ifstream infile(fname);
    if (!infile.is_open()) {
        return -1;
    }

    int numPosts;
    infile >> numPosts;

    int maxMessageId = -1;

    // read each post
    for (int i = 0; i < numPosts; i++) {

        // read the message ID
        int messageId;
        infile >> messageId;
        infile.ignore();

        // read the post message
        std::string message;
        std::getline(infile, message);
        message = message.substr(1); // remove tab

        // read the profile ID
        int profileId;
        infile >> profileId;

        // read the author ID
        int authorId;
        infile >> authorId;

        // read the number of likes
        int likes;
        infile >> likes;
        infile.ignore();

        // read the URL
        std::string url;
        std::getline(infile, url);
        if (!url.empty()) {
            url = url.substr(1); // remove tab
        }

        // create the Post object
        Post* post;

        // if the URL is empty, create a regular Post
        if (url.empty()) {
            post = new Post(profileId, authorId, message, likes);
        } else {
            post = new LinkPost(profileId, authorId, message, likes, url);
        }

        // ensure posts_ vector is large enough
        if (profileId >= posts_.size()) {
            posts_.resize(profileId + 1);
        }

        // set the message ID for the post
        post->setMessageId(messageId);
        if (messageId > maxMessageId) {
            maxMessageId = messageId;
        }
        posts_[profileId].push_back(post);
    }

    // close the input file
    infile.close();

    // ensure nextMessagesId_ is larger than any existing message id
    if (maxMessageId >= 0) {
        nextMessagesId_ = maxMessageId + 1;
    }
    return 0;
}


// writes posts to a file
int Network::writePosts(const char* fname) {
    std::ofstream outfile(fname);
    if (!outfile.is_open()) {
        return -1;
    }

    // gather all posts into a single vector
    std::vector<Post*> allPosts;

    // gather all posts from all users
    for (const auto& userPosts : posts_) {
        for (Post* p : userPosts) {
            allPosts.push_back(p);
        }
    }

    // sort the posts by message ID
    std::sort(allPosts.begin(), allPosts.end(), comparePosts);

    outfile << allPosts.size() << "\n";

    // write each post
    for (Post* p : allPosts) {

        // write the message ID
        outfile << p->getMessageId() << "\n";
        outfile << "\t" << p->getMessage() << "\n";
        outfile << "\t" << p->getProfileId() << "\n";
        outfile << "\t" << p->getAuthorId() << "\n";
        outfile << "\t" << p->getLikes() << "\n";

        // write the URL
        std::string url = p->getURL();
        if (url.empty()) {
            outfile << "\t\n";
        } else {
            outfile << "\t" << url << "\n";
        }
    }

    outfile.close();
    return 0;
}


// returns a string representation of the post
std::string Network::postDisplayString(Post* post) const {

    // get the author's name
    std::string authorName = getUser(post->getAuthorId())->getName();
    return authorName + " wrote: " + post->toString();
}

// get posts string for a user
std::string Network::getPostsString(int profileId, int howMany) const {
    if (profileId < 0 || profileId >= posts_.size()) {
        return "";
    }

    // get the user's posts
    const std::vector<Post*>& userPosts = posts_[profileId];
    std::string result = "";

    // get the number of posts to display
    int count = 0;
    for (int i = userPosts.size() - 1; i >= 0 && count < howMany; i--) {
        if (!result.empty()) {
            result += "\n\n";
        }
        result += postDisplayString(userPosts[i]);
        count++;
    }

    return result;
}


