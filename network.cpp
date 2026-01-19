#include "network.h"
#include <fstream>
#include <sstream>

Network::Network() {
}

// returns a pointer to the user with the given id or nullptr if not found
// id needs to be positive and greater than users_.size
User* Network::getUser(int id) {
    if (id < 0 || id >= users_.size()) {
        return nullptr;
    }
    return users_[id];
}

// adds a user to the network
void Network::addUser(User* user) {
    users_.push_back(user);
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