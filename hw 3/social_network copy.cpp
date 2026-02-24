#include <iostream>
#include <string>
#include "network.h"

int main(int argc, char* argv[]) {
    // filename argument
    if (argc < 3) {
        std::cout << "Usage: ./program users.txt posts.txt" << std::endl;
        return 1;
    }

    // read users from file
    Network net;
    net.readUsers(argv[1]);
    net.readPosts(argv[2]);


    while (true) {
        // display menu
    std::cout << std::endl;

        std::cout << "1. Add a user" << std::endl;
        std::cout << "2. Add friend connection" << std::endl;
        std::cout << "3. Delete friend connection" << std::endl;
        std::cout << "4. Write to file" << std::endl;
        std::cout << "5. Display posts" << std::endl;
        std::cout << "Enter option: ";

        int option;
        std::cin >> option;

        // exit on any invalid option
        if (option < 1 || option > 5) {
            break;
        }

        // OPTION 1: Add user
        if (option == 1) {
            std::string first, last;
            int year, zip;

            std::cin >> first >> last >> year >> zip;
            std::string name = first + " " + last;

            // id is next available index
            int id = net.numUsers();

            User* u = new User(id, name, year, zip, std::set<int>());
            net.addUser(u);
        }

        // OPTION 2: Add friend connection
        else if (option == 2) {

            // read first and last names of both users
            std::string f1, l1, f2, l2;
            std::cin >> f1 >> l1 >> f2 >> l2;

            // makes them strings
            std::string name1 = f1 + " " + l1;
            std::string name2 = f2 + " " + l2;

            // if either user doesn't exist
            if (net.addConnection(name1, name2) == -1) {
                    std::cout << "Error: user not found" << std::endl;
            }
        }

        // OPTION 3: Delete friend connection
        else if (option == 3) {

            // read first and last names of both users
            std::string f1, l1, f2, l2;
            std::cin >> f1 >> l1 >> f2 >> l2;

            // makes them strings
            std::string name1 = f1 + " " + l1;
            std::string name2 = f2 + " " + l2;

            // if either user doesn't exist
            if (net.deleteConnection(name1, name2) == -1) {
                std::cout << "Error: user not found or not friends" << std::endl;
            }
        }

        // OPTION 4: Write to file
        else if (option == 4) {
            // read filename
            std::string filename;
            std::cin >> filename;

            // write users to file
            net.writeUsers(filename.c_str());
            std::cout << net.numUsers() << " users written to file" << std::endl;
        }

        // OPTION 5

        // display posts
        else if (option == 5) {
            std::string first, last;
            int howMany;

            // read first and last names
            std::cin >> first >> last >> howMany;
            std::string name = first + " " + last;

            // get user id
            int id = net.getId(name);
            if (id != -1) {
                std::cout << net.getPostsString(id, howMany) << std::endl;
            }
        }
}

    return 0;
}

