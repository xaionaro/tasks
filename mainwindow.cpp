#include "mainwindow.h"
#include "helpwindow.h"
#include "ui_mainwindow.h"
#include "common.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->updateTasks();
}

int MainWindow::updateTasks() {
    redmine->request(RedmineClient::GET, "/issues.json?"+issues_filter, "");

    return 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionHelp_triggered()
{
    HelpWindow *win = new HelpWindow();
    win->show();
    return;
}
