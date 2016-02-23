#include "mainwindowandroid.h"
#include "ui_mainwindowandroid.h"

#include "logtimewindow.h"

MainWindowAndroid::MainWindowAndroid(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MainWindowAndroid)
{
    ui->setupUi(this);
}

MainWindowAndroid::~MainWindowAndroid()
{
    delete ui;
}

void MainWindowAndroid::on_quitButton_clicked()
{
    qApp->quit();
}

void MainWindowAndroid::on_logTimeWindowButton_clicked()
{
    LogTimeWindow *w = new LogTimeWindow();
    w->show();
}
