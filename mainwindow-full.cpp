/*
    mephi-tasks — a client to NRNU MEPhI Redmine server

    Copyright (C) 2015  Dmitry Yu Okunev <dyokunev@ut.mephi.ru> 0x8E30679C

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

#include <QResizeEvent>

#include "mainwindow-full.h"
#include "ui_mainwindow-full.h"

MainWindowFull::MainWindowFull(QWidget *parent) :
    MainWindowCommon(parent),
    ui(new Ui::MainWindowFull)
{
    ui->setupUi(this);

    this->setWindowTitle("Система «Задачи» НИЯУ МИФИ");

    this->setCorner(Qt::TopLeftCorner,    Qt::LeftDockWidgetArea);
    this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

    this->setDockOptions(AllowTabbedDocks | AllowNestedDocks);

    ui->navigationDock->installEventFilter(this);
    ui->issueDock->installEventFilter(this);
    ui->filtersDock->installEventFilter(this);

    this->setMinimumWidth(1280);

    ui->navigationDock->setMinimumWidth(this->navigationDockInitialWidth);

    return;
}


void MainWindowFull::on_resize_navigationDock(QResizeEvent *event) {
    /*if (event->oldSize().width() != -1 && event->size().width() != event->oldSize().width())
        if (event->size().width() - event->oldSize().width() > -(this->navigationDockInitialWidth)/2) */
            ui->navigationDock->setMinimumWidth(0);


    qDebug("navigationDock Resized (New Size) - Width: %d Height: %d (was: %d x %d)",
        event->size().width(),
        event->size().height(),
        event->oldSize().width(),
        event->oldSize().height()
     );

    return;
}

void MainWindowFull::on_resize_issueDock(QResizeEvent *event) {
    /*
    qDebug("issueDock Resized (New Size) - Width: %d Height: %d",
        event->size().width(),
        event->size().height());
*/
    return;
}

void MainWindowFull::on_resize_filtersDock(QResizeEvent *event) {
    /*
    qDebug("filtersDock Resized (New Size) - Width: %d Height: %d",
        event->size().width(),
        event->size().height());
*/
    return;
}

bool MainWindowFull::eventFilter(QObject *obj, QEvent *event) {    
    if (event->type() == QEvent::Resize) {
        Ui::MainWindowFull *ui = this->ui;
        if (obj == ui->navigationDock)
            this->on_resize_navigationDock(static_cast<QResizeEvent*>(event));
        else
        if (obj == ui->issueDock)
            this->on_resize_issueDock     (static_cast<QResizeEvent*>(event));
        else
        if (obj == ui->filtersDock)
            this->on_resize_filtersDock   (static_cast<QResizeEvent*>(event));
    }
    return QWidget::eventFilter(obj, event);
}

MainWindowFull::~MainWindowFull()
{
    delete ui;
}
