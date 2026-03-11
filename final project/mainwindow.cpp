#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentUserId(-1),
    viewingUserId(-1)
{
    ui->setupUi(this);

    network.readUsers("users.txt");
    network.readPosts("posts.txt");

    ui->profilePage->hide();

    // Buttons
    connect(ui->loginButton, &QPushButton::clicked,
            this, &MainWindow::handleLogin);

    connect(ui->addFriendButton, &QPushButton::clicked,
            this, &MainWindow::handleAddFriend);

    connect(ui->addPostButton, &QPushButton::clicked,
            this, &MainWindow::handleAddPost);

    connect(ui->backButton, &QPushButton::clicked,
            this, &MainWindow::handleBack);

    // Clicking friend navigates
    connect(ui->friendsTable, &QTableWidget::cellClicked,
            this, &MainWindow::handleFriendClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::handleLogin()
{
    QString name = ui->nameInput->toPlainText().trimmed();

    int id = network.getId(name.toStdString());

    if (id == -1) {
        QMessageBox::warning(this, "Error", "User not found.");
        return;
    }

    currentUserId = id;
    viewingUserId = id;

    ui->loginPage->hide();
    ui->profilePage->show();

    ui->profileLabel->setText("My Profile");

    populateFriends();
    populateSuggestions();
    populatePosts(viewingUserId);
}

void MainWindow::handleFriendClicked(int row, int)
{
    QTableWidgetItem* item = ui->friendsTable->item(row, 0);
    if (!item) return;

    QString name = item->text();
    int id = network.getId(name.toStdString());

    if (id == -1) return;

    viewingUserId = id;

    ui->profileLabel->setText(name + "'s Profile");

    populatePosts(viewingUserId);
    populateSuggestions();
}


void MainWindow::handleBack()
{
    viewingUserId = currentUserId;

    ui->profileLabel->setText("My Profile");

    populatePosts(viewingUserId);
    populateFriends();
    populateSuggestions();
}



void MainWindow::populateFriends()
{
    ui->friendsTable->clear();
    ui->friendsTable->setColumnCount(1);
    ui->friendsTable->setHorizontalHeaderLabels(QStringList() << "Friends");

    User* user = network.getUser(currentUserId);
    std::set<int> friends = user->getFriends();

    ui->friendsTable->setRowCount(friends.size());

    int row = 0;
    for (int id : friends) {
        User* f = network.getUser(id);

        ui->friendsTable->setItem(
            row,
            0,
            new QTableWidgetItem(
                QString::fromStdString(f->getName())
                )
            );
        row++;
    }

    ui->friendsTable->resizeColumnsToContents();
}


void MainWindow::populateSuggestions()
{
    ui->suggestionsTable->clear();
    ui->suggestionsTable->setColumnCount(1);
    ui->suggestionsTable->setHorizontalHeaderLabels(QStringList() << "Suggestions");

    int score;
    std::vector<int> suggestions =
        network.suggestFriends(currentUserId, score);

    ui->suggestionsTable->setRowCount(suggestions.size());

    for (int i = 0; i < suggestions.size(); i++) {
        User* u = network.getUser(suggestions[i]);

        ui->suggestionsTable->setItem(
            i,
            0,
            new QTableWidgetItem(
                QString::fromStdString(u->getName())
                )
            );
    }

    ui->suggestionsTable->resizeColumnsToContents();
}


void MainWindow::populatePosts(int profileId)
{
    ui->postsLabel->clear();

    int mutual = 0;
    if (currentUserId >= 0) {
        mutual = network.getMutualFriendCount(currentUserId, profileId);
    }

    std::string name = "";
    User* u = network.getUser(profileId);
    if (u != nullptr) {
        name = u->getName();
    }

    ui->profileLabel->setText(
        "Viewing " + QString::fromStdString(name) + " | Mutual Friends: " + QString::number(mutual)
    );

    QString postsText =
        QString::fromStdString(
            network.getPostsString(profileId, 10)
            );

    ui->postsLabel->setText(postsText);
}


void MainWindow::handleAddFriend()
{
    int row = ui->suggestionsTable->currentRow();

    if (row < 0)
        return;

    QString friendName =
        ui->suggestionsTable->item(row, 0)->text();

    network.addConnection(
        network.getUser(currentUserId)->getName(),
        friendName.toStdString()
        );

    network.writeUsers("users.txt");

    populateFriends();
    populateSuggestions();
}



void MainWindow::handleAddPost()
{
    QString text =
        ui->postInput->toPlainText().trimmed();

    if (text.isEmpty())
        return;

    Post* post = new Post(
        viewingUserId,     // profile being viewed
        currentUserId,     // author
        text.toStdString(),
        0
        );

    network.addPost(post);
    network.writePosts("posts.txt");

    ui->postInput->clear();

    populatePosts(viewingUserId);
}

int MainWindow::getSelectedPostId() const {
    return -1;
}

std::string MainWindow::getSelectedFriendName() const {
    int row = ui->friendsTable->currentRow();
    if (row < 0) return "";
    QTableWidgetItem* item = ui->friendsTable->item(row, 0);
    if (!item) return "";
    return item->text().toStdString();
}

void MainWindow::handleLikePost()
{
    int postId = getSelectedPostId();

    Post* p = network.getPost(postId);

    if (p && p->addLike(currentUserId)) {

        network.writePosts("posts.txt");

    }

    populatePosts(viewingUserId);
}

void MainWindow::handleUnlikePost()
{
    int postId = getSelectedPostId();

    Post* p = network.getPost(postId);

    if (p && p->removeLike(currentUserId)) {

        network.writePosts("posts.txt");

    }

    populatePosts(viewingUserId);
}

void MainWindow::handleDeletePost()
{
    int postId = getSelectedPostId();

    Post* p = network.getPost(postId);

    if (p && p->getAuthorId() == currentUserId) {

        network.deletePost(postId);

        network.writePosts("posts.txt");

        populatePosts(viewingUserId);
    }
}

void MainWindow::handleRemoveFriend()
{
    std::string friendName = getSelectedFriendName();

    if (friendName.empty()) return;

    network.removeConnection(network.getUser(currentUserId)->getName(), friendName);

    network.writeUsers("users.txt");

    populateFriends();
}
