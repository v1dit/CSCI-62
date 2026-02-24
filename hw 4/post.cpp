#include "post.h"

#include <sstream>


Post::Post()
{
    messageId_ = 0;
    profileId_ = 0;
    authorId_ = 0;
    message_ = "";
    likes_ = 0;
}

Post::Post(int profileId, int authorId, std::string message, int likes)
{
    messageId_ = 0;
    profileId_ = profileId;
    authorId_ = authorId;
    message_ = message;
    likes_ = likes;
}

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

std::string Post::getMessage() const {
    return message_;
}

int Post::getLikes() const {
    return likes_;
}

std::string Post::getURL() const {
    return "";
}

std::string Post::toString() const {
    std::stringstream ss;
    ss << message_;
    return ss.str();
}



LinkPost::LinkPost() : Post()
{
    url_ = "";
}

LinkPost::LinkPost(int profileId, int authorId,
                   std::string message,
                   int likes,
                   std::string url)
    : Post(profileId, authorId, message, likes)
{
    url_ = url;
}

std::string LinkPost::getURL() const {
    return url_;
}

std::string LinkPost::toString() const {
    return getMessage() + " " + url_;
}
