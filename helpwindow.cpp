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


#include "helpwindow.h"
#include "ui_helpwindow.h"

HelpWindow::HelpWindow ( QWidget *parent ) :
	QDialog ( parent ),
	ui ( new Ui::HelpWindow )
{
	ui->setupUi ( this );
}

HelpWindow::~HelpWindow()
{
	delete ui;
}

void HelpWindow::on_pushButton_clicked()
{
	delete this;
}
