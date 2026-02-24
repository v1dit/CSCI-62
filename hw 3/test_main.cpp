#include <iostream>
#include "post.h"
#include "user.h"
#include "network.h"

int main() {
    // Test Post default
    Post p1;
    std::cout << "Post default messageId: " << p1.getMessageId() << " (expected -1)\n";

    // Test parameterized
    Post p2(1, 2, "hello", 3);
    std::cout << "Post p2 profile: " << p2.getProfileId() << " author: " << p2.getAuthorId() << " message: " << p2.getMessage() << " likes: " << p2.getLikes() << "\n";

    // LinkPost
    LinkPost lp(0, 1, "link", 5, "http://a.com");
    std::cout << "LinkPost url: " << lp.getURL() << "\n";
    std::cout << "LinkPost toString: " << lp.toString() << "\n";

    // Test Network addPost
    Network net;
    User* u = new User(0, "A B", 2000, 12345, std::set<int>());
    net.addUser(u);

    Post* p3 = new Post(0, 0, "msg1", 0);
    net.addPost(p3);
    Post* p4 = new Post(0, 1, "msg2", 1);
    net.addPost(p4);

    auto posts = net.getPosts(0);
    std::cout << "posts size: " << posts.size() << "\n";
    for (auto pp : posts) {
        std::cout << "id=" << pp->getMessageId() << " text=" << pp->getMessage() << "\n";
    }

    return 0;
}
