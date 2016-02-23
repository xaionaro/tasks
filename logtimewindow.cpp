#include "logtimewindow.h"
#include "ui_logtimewindow.h"

LogTimeWindow::LogTimeWindow(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::LogTimeWindow)
{
    ui->setupUi(this);
}

LogTimeWindow::~LogTimeWindow()
{
    delete ui;
}

void LogTimeWindow::on_cancel_clicked()
{
    delete this;
}

void LogTimeWindow::on_accept_clicked()
{
    delete this;
}
