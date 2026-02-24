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
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *myButton;
    QLabel *countLabel;
    QLabel *winLabel;
    QLabel *instructionsLabel;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        myButton = new QPushButton(centralwidget);
        myButton->setObjectName("myButton");
        myButton->setGeometry(QRect(240, 100, 100, 32));
        countLabel = new QLabel(centralwidget);
        countLabel->setObjectName("countLabel");
        countLabel->setGeometry(QRect(230, 50, 131, 20));
        winLabel = new QLabel(centralwidget);
        winLabel->setObjectName("winLabel");
        winLabel->setEnabled(true);
        winLabel->setGeometry(QRect(270, 200, 58, 16));
        instructionsLabel = new QLabel(centralwidget);
        instructionsLabel->setObjectName("instructionsLabel");
        instructionsLabel->setGeometry(QRect(30, 10, 58, 16));
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
        myButton->setText(QCoreApplication::translate("MainWindow", "Click Me", nullptr));
        countLabel->setText(QCoreApplication::translate("MainWindow", "Number of Clicks: 0", nullptr));
        winLabel->setText(QCoreApplication::translate("MainWindow", "You Win", nullptr));
        instructionsLabel->setText(QCoreApplication::translate("MainWindow", "temp", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
