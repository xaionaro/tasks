#include "syntaxwindow.h"
#include "ui_syntaxwindow.h"

SyntaxWindow::SyntaxWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SyntaxWindow)
{
    ui->setupUi(this);
}

SyntaxWindow::~SyntaxWindow()
{
    delete ui;
}

void SyntaxWindow::on_closeButton_clicked()
{
    qApp->quit();
}
