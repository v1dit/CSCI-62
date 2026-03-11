#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "network.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleLogin();
    void handleAddFriend();
    void handleAddPost();

    void handleLikePost();
    void handleUnlikePost();
    void handleDeletePost();
    void handleRemoveFriend();

    void handleFriendClicked(int row, int column);
    void handleBack();

private:
    void populateFriends();
    void populateSuggestions();
    void populatePosts(int profileId);

    int getSelectedPostId() const;
    std::string getSelectedFriendName() const;

    Ui::MainWindow *ui;

    Network network;

    int currentUserId;
    int viewingUserId;
};

#endif
