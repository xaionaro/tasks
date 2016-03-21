#include "signingwindow.h"
#include "ui_signingwindow.h"

SigningWindow::SigningWindow ( QWidget *parent ) :
	QDialog ( parent ),
	ui ( new Ui::SigningWindow )
{
	ui->setupUi ( this );
}

SigningWindow::~SigningWindow()
{
	this->on_destructor();
	delete ui;
}
