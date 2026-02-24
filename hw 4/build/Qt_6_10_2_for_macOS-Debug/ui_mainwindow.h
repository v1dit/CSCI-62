/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *loginPage;
    QLabel *promptLabel;
    QTextEdit *nameInput;
    QPushButton *loginButton;
    QWidget *profilePage;
    QLabel *profileLabel;
    QTableWidget *friendsTable;
    QLabel *postsLabel;
    QPushButton *backButton;
    QTableWidget *suggestionsTable;
    QPushButton *addFriendButton;
    QTextEdit *postInput;
    QPushButton *addPostButton;
    QTableWidget *postsTable;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        loginPage = new QWidget(centralwidget);
        loginPage->setObjectName("loginPage");
        loginPage->setGeometry(QRect(110, 10, 561, 291));
        promptLabel = new QLabel(loginPage);
        promptLabel->setObjectName("promptLabel");
        promptLabel->setGeometry(QRect(220, 30, 101, 16));
        nameInput = new QTextEdit(loginPage);
        nameInput->setObjectName("nameInput");
        nameInput->setGeometry(QRect(220, 60, 104, 74));
        loginButton = new QPushButton(loginPage);
        loginButton->setObjectName("loginButton");
        loginButton->setGeometry(QRect(190, 140, 161, 32));
        profilePage = new QWidget(centralwidget);
        profilePage->setObjectName("profilePage");
        profilePage->setEnabled(true);
        profilePage->setGeometry(QRect(30, 0, 751, 541));
        profileLabel = new QLabel(profilePage);
        profileLabel->setObjectName("profileLabel");
        profileLabel->setGeometry(QRect(320, 30, 71, 16));
        friendsTable = new QTableWidget(profilePage);
        friendsTable->setObjectName("friendsTable");
        friendsTable->setGeometry(QRect(560, 410, 131, 91));
        postsLabel = new QLabel(profilePage);
        postsLabel->setObjectName("postsLabel");
        postsLabel->setGeometry(QRect(60, 50, 611, 351));
        backButton = new QPushButton(profilePage);
        backButton->setObjectName("backButton");
        backButton->setGeometry(QRect(560, 500, 141, 32));
        suggestionsTable = new QTableWidget(profilePage);
        suggestionsTable->setObjectName("suggestionsTable");
        suggestionsTable->setGeometry(QRect(400, 410, 131, 91));
        addFriendButton = new QPushButton(profilePage);
        addFriendButton->setObjectName("addFriendButton");
        addFriendButton->setGeometry(QRect(410, 500, 111, 32));
        postInput = new QTextEdit(profilePage);
        postInput->setObjectName("postInput");
        postInput->setGeometry(QRect(60, 410, 141, 91));
        addPostButton = new QPushButton(profilePage);
        addPostButton->setObjectName("addPostButton");
        addPostButton->setGeometry(QRect(250, 500, 100, 32));
        postsTable = new QTableWidget(profilePage);
        postsTable->setObjectName("postsTable");
        postsTable->setGeometry(QRect(230, 410, 141, 91));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 37));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        promptLabel->setText(QCoreApplication::translate("MainWindow", "Enter your name:", nullptr));
        loginButton->setText(QCoreApplication::translate("MainWindow", "Login", nullptr));
        profileLabel->setText(QCoreApplication::translate("MainWindow", "My Profile", nullptr));
        postsLabel->setText(QCoreApplication::translate("MainWindow", "postsLabel", nullptr));
        backButton->setText(QCoreApplication::translate("MainWindow", "Back to My Profile", nullptr));
        addFriendButton->setText(QCoreApplication::translate("MainWindow", "Add Friend", nullptr));
        addPostButton->setText(QCoreApplication::translate("MainWindow", "Add Post", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
