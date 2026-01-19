#include <iostream>
#include <string>
#include "network.h"

using namespace std;

int main(int argc, char* argv[]) {
    // filename argument
    if (argc < 2) {
        cout << "Usage: ./program users.txt" << endl;
        return 1;
    }

    // read users from file
    Network net;
    net.readUsers(argv[1]);

    while (true) {
        // display menu
        cout << endl;
        cout << "1. Add a user" << endl;
        cout << "2. Add friend connection" << endl;
        cout << "3. Delete friend connection" << endl;
        cout << "4. Write to file" << endl;
        cout << "Enter option: ";

        int option;
        cin >> option;

        // exit on any invalid option
        if (option < 1 || option > 4) {
            break;
        }

        // OPTION 1: Add user
        if (option == 1) {
            string first, last;
            int year, zip;

            cin >> first >> last >> year >> zip;
            string name = first + " " + last;

            // id is next available index
            int id = net.numUsers();

            User* u = new User(id, name, year, zip, std::set<int>());
            net.addUser(u);
        }

        // OPTION 2: Add friend connection
        else if (option == 2) {

            // read first and last names of both users
            string f1, l1, f2, l2;
            cin >> f1 >> l1 >> f2 >> l2;

            // makes them strings
            string name1 = f1 + " " + l1;
            string name2 = f2 + " " + l2;

            // if either user doesn't exist
            if (net.addConnection(name1, name2) == -1) {
                cout << "Error: user not found" << endl;
            }
        }

        // OPTION 3: Delete friend connection
        else if (option == 3) {

            // read first and last names of both users
            string f1, l1, f2, l2;
            cin >> f1 >> l1 >> f2 >> l2;

            // makes them strings
            string name1 = f1 + " " + l1;
            string name2 = f2 + " " + l2;

            // if either user doesn't exist
            if (net.deleteConnection(name1, name2) == -1) {
                cout << "Error: user not found or not friends" << endl;
            }
        }

        // OPTION 4: Write to file
        else if (option == 4) {
            // read filename
            string filename;
            cin >> filename;

            // write users to file
            net.writeUsers(filename.c_str());
            cout << net.numUsers() << " users written to file" << endl;
        }
    }

    return 0;
}

