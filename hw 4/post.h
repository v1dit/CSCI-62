#ifndef POST_H
#define POST_H

#include <string>

class Post {
public:
    Post();
    Post(int profileId, int authorId, std::string message, int likes);

    int getMessageId() const;
    void setMessageId(int id);

    int getProfileId() const;
    int getAuthorId() const;

    std::string getMessage() const;
    int getLikes() const;

    virtual std::string getURL() const;
    virtual std::string toString() const;

    virtual ~Post() {}

private:
    int messageId_;
    int profileId_;
    int authorId_;
    std::string message_;
    int likes_;
};

class LinkPost : public Post {
public:
    LinkPost();
    LinkPost(int profileId, int authorId, std::string message, int likes, std::string url);

    std::string getURL() const override;
    std::string toString() const override;

private:
    std::string url_;
};

#endif
