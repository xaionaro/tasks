/*
    tasks — a Qt-based client to an ITS

    Copyright (C) 2015-2020  Dmitrii Okunev <xaionaro@dx.center>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow ( QDialog *parent ) : QDialog ( parent ),
	ui ( new Ui::LoginWindow )
{
	this->ui->setupUi ( this );
	this->ui->password->setEchoMode(QLineEdit::Password);
	this->resultApiKey = "";
}


LoginWindow::~LoginWindow()
{
	//delete ui;
}

void LoginWindow::on_buttonBox_accepted()
{
    Redmine *redmine;
    redmine = new Redmine(this->ui->url->text());
    redmine->setAuth ( this->ui->login->text(), this->ui->password->text() );
    redmine->init();

    if ( !redmine->me().isEmpty() ) {
        this->resultApiKey = redmine->me() ["api_key"].toString();
		qDebug ( "Received information about me. Result apiKey: %s", this->resultApiKey.toStdString().c_str() );
	}
    delete redmine;
}

void LoginWindow::on_buttonBox_rejected()
{
}
