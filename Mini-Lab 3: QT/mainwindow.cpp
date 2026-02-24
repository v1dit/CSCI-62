#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->winLabel->hide();

    connect(ui->myButton, &QPushButton::clicked,
            this, &MainWindow::onMyButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onMyButtonClicked()
{
    counter.add();

    int count = counter.getCount();

    ui->countLabel->setText(
        "Number of Clicks: " + QString::number(count)
        );

    if (count >= 10) {
        ui->myButton->hide();
        ui->countLabel->hide();
        ui->winLabel->show();
    }
}
