#include "mainwindow-full.h"
#include "ui_mainwindow-full.h"

MainWindowFull::MainWindowFull(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowFull)
{
    ui->setupUi(this);
    this->setCorner(Qt::TopLeftCorner,    Qt::LeftDockWidgetArea);
    this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
}

MainWindowFull::~MainWindowFull()
{
    delete ui;
}
