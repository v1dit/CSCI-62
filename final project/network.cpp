#include "network.h"
#include "post.h"

#include <fstream>
#include <sstream>
#include <set>
#include <queue>
#include <stack>
#include <algorithm>
#include <iostream>

Network::Network() {
    nextMessagesId_ = 0;
}


User* Network::getUser(int id) const {
    if (id < 0 || id >= (int)users_.size()) {
        return nullptr;
    }
    return users_[id];
}

void Network::addUser(User* user) {
    users_.push_back(user);
    if (posts_.size() < users_.size()) {
        posts_.resize(users_.size());
    }
}

int Network::getId(std::string name) {
    for (int i = 0; i < (int)users_.size(); i++) {
        if (users_[i]->getName() == name) {
            return i;
        }
    }
    return -1;
}

int Network::addConnection(std::string s1, std::string s2) {
    int id1 = getId(s1);
    int id2 = getId(s2);

    if (id1 == -1 || id2 == -1) {
        return -1;
    }

    users_[id1]->addFriend(id2);
    users_[id2]->addFriend(id1);
    return 0;
}

int Network::deleteConnection(std::string s1, std::string s2) {
    int id1 = getId(s1);
    int id2 = getId(s2);

    if (id1 == -1 || id2 == -1) {
        return -1;
    }

    users_[id1]->deleteFriend(id2);
    users_[id2]->deleteFriend(id1);
    return 0;
}

int Network::numUsers() {
    return users_.size();
}


void Network::readUsers(const char* fname) {

    std::ifstream infile(fname);
    if (!infile.is_open()) {
        std::cout << "Could not open users file\n";
        return;
    }

    users_.clear();

    int n;
    infile >> n;
    infile.ignore();

    for (int i = 0; i < n; i++) {

        int id;
        infile >> id;
        infile.ignore();

        std::string name;
        std::getline(infile, name);

        name.erase(0, name.find_first_not_of(" \t\r\n"));
        name.erase(name.find_last_not_of(" \t\r\n") + 1);

        int year;
        infile >> year;
        infile.ignore();

        int zip;
        infile >> zip;
        infile.ignore();

        std::string line;
        std::getline(infile, line);

        std::set<int> friends;
        std::stringstream ss(line);
        int fid;
        while (ss >> fid) {
            friends.insert(fid);
        }

        User* u = new User(id, name, year, zip, friends);
        users_.push_back(u);
    }

    if (posts_.size() < users_.size()) {
        posts_.resize(users_.size());
    }
}


void Network::writeUsers(const char* fname) {

    std::ofstream outfile(fname);
    if (!outfile.is_open()) {
        return;
    }

    outfile << users_.size() << "\n";

    for (User* u : users_) {

        outfile << u->getId() << "\n";
        outfile << "\t" << u->getName() << "\n";
        outfile << "\t" << u->getYear() << "\n";
        outfile << "\t" << u->getZip() << "\n";

        outfile << "\t";
        for (int fid : u->getFriends()) {
            outfile << fid << " ";
        }
        outfile << "\n";
    }
}


std::vector<int> Network::shortestPath(int from, int to) {

    std::vector<int> path;

    if (from < 0 || from >= (int)users_.size() ||
        to < 0 || to >= (int)users_.size()) {
        return path;
    }

    std::vector<bool> visited(users_.size(), false);
    std::vector<int> prev(users_.size(), -1);
    std::queue<int> q;

    visited[from] = true;
    q.push(from);

    while (!q.empty()) {

        int curr = q.front();
        q.pop();

        if (curr == to) {

            for (int x = to; x != -1; x = prev[x]) {
                path.push_back(x);
            }

            std::reverse(path.begin(), path.end());
            return path;
        }

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


std::vector<int> Network::suggestFriends(int who, int& score) {

    std::vector<int> result;
    score = 0;

    if (who < 0 || who >= (int)users_.size()) {
        return result;
    }

    const std::set<int>& myFriends = users_[who]->getFriends();

    for (int i = 0; i < (int)users_.size(); i++) {

        if (i != who && myFriends.count(i) == 0) {

            int common = 0;
            const std::set<int>& theirFriends = users_[i]->getFriends();

            for (int f : myFriends) {
                if (theirFriends.count(f)) {
                    common++;
                }
            }

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

    if (score == 0) {
        result.clear();
    }

    return result;
}


void Network::addPost(Post* post) {

    int profileId = post->getProfileId();

    if (profileId >= (int)posts_.size()) {
        posts_.resize(profileId + 1);
    }

    post->setMessageId(nextMessagesId_);
    nextMessagesId_++;

    posts_[profileId].push_back(post);
}

std::vector<Post*> Network::getPosts(int id) const {

    if (id < 0 || id >= (int)posts_.size()) {
        return {};
    }

    return posts_[id];
}

static bool comparePosts(Post* a, Post* b) {
    return a->getMessageId() < b->getMessageId();
}

int Network::readPosts(const char* fname) {

    std::ifstream infile(fname);
    if (!infile.is_open()) {
        return -1;
    }

    posts_.clear();

    int numPosts;
    infile >> numPosts;

    int maxMessageId = -1;

    for (int i = 0; i < numPosts; i++) {

        int messageId;
        infile >> messageId;
        infile.ignore();

        std::string message;
        std::getline(infile, message);
        message.erase(0, message.find_first_not_of(" \t\r\n"));

        int profileId;
        infile >> profileId;

        int authorId;
        infile >> authorId;

        int likes;
        infile >> likes;
        infile.ignore();

        std::string url;
        std::getline(infile, url);
        url.erase(0, url.find_first_not_of(" \t\r\n"));

        Post* post;

        if (url.empty()) {
            post = new Post(profileId, authorId, message, likes);
        }
        else {
            post = new LinkPost(profileId, authorId, message, likes, url);
        }

        if (profileId >= (int)posts_.size()) {
            posts_.resize(profileId + 1);
        }

        post->setMessageId(messageId);

        if (messageId > maxMessageId) {
            maxMessageId = messageId;
        }

        posts_[profileId].push_back(post);
    }

    if (maxMessageId >= 0) {
        nextMessagesId_ = maxMessageId + 1;
    }

    return 0;
}

int Network::writePosts(const char* fname) {

    std::ofstream outfile(fname);
    if (!outfile.is_open()) {
        return -1;
    }

    std::vector<Post*> allPosts;

    for (const auto& userPosts : posts_) {
        for (Post* p : userPosts) {
            allPosts.push_back(p);
        }
    }

    std::sort(allPosts.begin(), allPosts.end(), comparePosts);

    outfile << allPosts.size() << "\n";

    for (Post* p : allPosts) {

        outfile << p->getMessageId() << "\n";
        outfile << "\t" << p->getMessage() << "\n";
        outfile << "\t" << p->getProfileId() << "\n";
        outfile << "\t" << p->getAuthorId() << "\n";
        outfile << "\t" << p->getLikes() << "\n";

        std::string url = p->getURL();
        if (url.empty()) {
            outfile << "\t\n";
        }
        else {
            outfile << "\t" << url << "\n";
        }
    }

    return 0;
}

bool Network::deletePost(int postId) {

    for (int i = 0; i < (int)posts_.size(); i++) {

        for (int j = 0; j < (int)posts_[i].size(); j++) {

            if (posts_[i][j]->getMessageId() == postId) {

                delete posts_[i][j];
                posts_[i].erase(posts_[i].begin() + j);

                return true;
            }
        }
    }

    return false;
}

void Network::removeConnection(std::string name1, std::string name2) {

    int id1 = getId(name1);
    int id2 = getId(name2);

    if (id1 == -1 || id2 == -1) {
        return;
    }

    users_[id1]->deleteFriend(id2);
    users_[id2]->deleteFriend(id1);
}

int Network::getMutualFriendCount(int id1, int id2) {

    if (id1 < 0 || id1 >= (int)users_.size() || id2 < 0 || id2 >= (int)users_.size()) {
        return 0;
    }

    const std::set<int>& f1 = users_[id1]->getFriends();
    const std::set<int>& f2 = users_[id2]->getFriends();

    int count = 0;

    for (int f : f1) {
        if (f2.count(f)) {
            count++;
        }
    }

    return count;
}

Post* Network::getPost(int postId) const {

    for (const auto& userPosts : posts_) {
        for (Post* p : userPosts) {
            if (p->getMessageId() == postId) {
                return p;
            }
        }
    }

    return nullptr;
}

std::string Network::getPostsString(int profileId, int howMany) const
{
    if (profileId < 0 || profileId >= (int)posts_.size()) {
        return "";
    }

    const std::vector<Post*>& userPosts = posts_[profileId];

    std::string result = "";
    int count = 0;

    // show most recent posts first
    for (int i = userPosts.size() - 1;
         i >= 0 && count < howMany;
         i--)
    {
        if (!result.empty()) {
            result += "\n\n";
        }

        result += postDisplayString(userPosts[i]);
        count++;
    }

    return result;
}

std::string Network::postDisplayString(Post* post) const
{
    if (post == nullptr) {
        return "";
    }

    User* author = getUser(post->getAuthorId());

    std::string authorName = "";
    if (author != nullptr) {
        authorName = author->getName();
    }

    std::string result = authorName + "\n";
    result += post->getMessage() + "\n";
    result += "Likes: " + std::to_string(post->getLikes());

    std::string url = post->getURL();
    if (!url.empty()) {
        result += "\n" + url;
    }

    return result;
}

