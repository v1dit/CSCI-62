#include "post.h"
#include <string>

using namespace std;

// Default constructor
Post::Post()
    : messageId_(-1), profileId_(0), authorId_(0),
      message_(""), likes_(0) {}

// Parameterized constructor
Post::Post(int profileId, int authorId, string message, int likes)
    : messageId_(-1), profileId_(profileId), authorId_(authorId),
      message_(message), likes_(likes) {}

int Post::getMessageId() const {
    return messageId_;
}

void Post::setMessageId(int id) {
    messageId_ = id;
}

int Post::getProfileId() const {
    return profileId_;
}

int Post::getAuthorId() const {
    return authorId_;
}

string Post::getMessage() const {
    return message_;
}

int Post::getLikes() const {
    return likes_;
}

string Post::getURL() const {
    return "";
}

// square brackets per spec
string Post::toString() const {
    return message_ + " (" + to_string(likes_) + " likes)";
}


// Default constructor
LinkPost::LinkPost()
    : Post(), url_("") {}

// Parameterized constructor
LinkPost::LinkPost(int profileId, int authorId, string message, int likes, string url)
    : Post(profileId, authorId, message, likes), url_(url) {}

string LinkPost::getURL() const {
    return url_;
}

string LinkPost::toString() const {
    return getMessage() + " (url: " + url_ + ") (" +
           to_string(getLikes()) + " likes)";
}


