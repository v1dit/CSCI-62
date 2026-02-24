#ifndef POST_H
#define POST_H

#include <string>

class Post {

public:

    // pre: none
    // post: constructs a Post object
    Post();
    // pre: profileId, authorId >= 0; message is not empty; likes >= 0
    // post: constructs a Post object
    Post(int profileId, int authorId, std::string message, int likes);

    // pre: none
    // post: returns the message id
    int getMessageId() const;
    void setMessageId(int id);

    // pre: none
    // post: returns the profile id
    int getProfileId() const;
    int getAuthorId() const;

    // pre: none
    // post: returns the message
    std::string getMessage() const;
    int getLikes() const;

    // pre: none
    // post: returns the URL
    virtual std::string getURL() const;
    virtual std::string toString() const;

private:
    int messageId_;
    int profileId_;
    int authorId_;
    std::string message_;
    int likes_;
};

class LinkPost : public Post {
public:

    // pre: none
    // post: constructs a LinkPost object
    LinkPost();
    // pre: profileId, authorId >= 0; message is not empty; likes >= 0; url is not empty
    // post: constructs a LinkPost object
    LinkPost(int profileId, int authorId, std::string message, int likes, std::string url);

    // pre: none
    // post: returns the URL
    std::string getURL() const override;
    // pre: none
    // post: returns a string representation of the LinkPost object
    std::string toString() const override;

private:
    std::string url_;
};

#endif
