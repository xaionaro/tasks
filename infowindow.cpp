#include "infowindow.h"
#include "ui_infowindow.h"

InfoWindow::InfoWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoWindow)
{
    ui->setupUi(this);
}

InfoWindow::~InfoWindow()
{
    delete ui;
}

int InfoWindow::setMessageText(QString text) {
    ui->message->setText(text);
    return 0;
}

void InfoWindow::on_closeButton_clicked()
{
    delete this;
}
