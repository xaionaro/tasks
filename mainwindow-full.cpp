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

#include "helpwindow.h"
#include "htmldelegate.h"

MainWindowFull::MainWindowFull(QWidget *parent) :
    MainWindowCommon(parent),
    ui(new Ui::MainWindowFull)
{
    this->ui->setupUi(this);

    connect(redmine, SIGNAL(callback_call      (void*,callback_t,QNetworkReply*,QJsonDocument*,void*)),
            this,    SLOT(  callback_dispatcher(void*,callback_t,QNetworkReply*,QJsonDocument*,void*)) );

    this->setWindowTitle("Система «Задачи» НИЯУ МИФИ");

    this->setCorner(Qt::TopLeftCorner,    Qt::LeftDockWidgetArea);
    this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

    this->setDockOptions(AllowTabbedDocks | AllowNestedDocks);

    this->ui->navigationDock->installEventFilter(this);
    this->ui->issueDock->installEventFilter(this);
    this->ui->filtersDock->installEventFilter(this);
    this->ui->centralWidget->installEventFilter(this);

    this->ui->navigationDock->setMinimumWidth(this->navigationDockInitialWidth);
    this->ui->filtersDock->setMinimumWidth(this->filtersDockInitialWidth);
    this->ui->issueDock->setMinimumHeight(this->issueDockInitialHeight);

    QStringList projectsColumns;
    projectsColumns << "Проект";
    this->ui->projects->setHeaderLabels(projectsColumns);
    HTMLDelegate* delegate = new HTMLDelegate();
    ui->projects->setItemDelegate(delegate);

    QStringList issuesColumns;
    issuesColumns << "Название" << "Исполнитель" << "Срок" << "Статус" << "Обновлено";
    this->ui->issuesTree->setHeaderLabels(issuesColumns);

    this->updateIssues();
    this->updateProjects();

    this->timerUpdateIssues = new QTimer(this);
    connect(this->timerUpdateIssues, SIGNAL(timeout()), this, SLOT(updateIssues()));
    this->timerUpdateIssues->start(10000);

    this->timerUpdateProjects = new QTimer(this);
    connect(this->timerUpdateProjects, SIGNAL(timeout()), this, SLOT(updateProjects()));
    this->timerUpdateProjects->start(5000);

    return;
}

MainWindowFull::~MainWindowFull()
{
    delete ui;
}

/**** ui ****/

void unlockDockWidth(QDockWidget *widget, QResizeEvent *event, int initialWidth) {
    if (event->oldSize().width() != -1 && event->size().width() != event->oldSize().width())
        if (event->size().width() - event->oldSize().width() > -initialWidth/2)
            widget->setMinimumWidth(0);
}

void unlockDockHeight(QDockWidget *widget, QResizeEvent *event, int initialHeight) {
    if (event->oldSize().height() != -1 && event->size().height() != event->oldSize().height())
        if (event->size().height() - event->oldSize().height() > -initialHeight/2)
            widget->setMinimumHeight(0);
}

void MainWindowFull::on_resize_centralWidget(QResizeEvent *event) {
/*
    qDebug("centralWidget Resized (New Size) - Width: %d Height: %d (was: %d x %d)",
        event->size().width(),
        event->size().height(),
        event->oldSize().width(),
        event->oldSize().height()
     );*/

    this->ui->issuesLayout->setGeometry(QRect(0, 0, event->size().width(), event->size().height()-10));

    return;
}

void MainWindowFull::on_resize_navigationDock(QResizeEvent *event) {
    unlockDockWidth(this->ui->navigationDock, event, this->navigationDockInitialWidth);
/*
    qDebug("navigationDock Resized (New Size) - Width: %d Height: %d (was: %d x %d)",
        event->size().width(),
        event->size().height(),
        event->oldSize().width(),
        event->oldSize().height()
     );*/

    this->ui->navigationTabs->resize(event->size().width(), event->size().height()-20);
    this->ui->projects->resize(event->size().width(), event->size().height()-60);

    return;
}

void MainWindowFull::on_resize_issueDock(QResizeEvent *event) {
    unlockDockHeight(this->ui->issueDock, event, this->issueDockInitialHeight);
/*
    qDebug("issueDock Resized (New Size) - Width: %d Height: %d",
        event->size().width(),
        event->size().height());*/

    this->ui->issueLayout->setGeometry(QRect(0, 0, event->size().width(), event->size().height()-10));

    return;
}

void MainWindowFull::on_resize_filtersDock(QResizeEvent *event) {
    unlockDockWidth(this->ui->filtersDock, event, this->filtersDockInitialWidth);
/*
    qDebug("filtersDock Resized (New Size) - Width: %d Height: %d",
        event->size().width(),
        event->size().height());*/

    this->ui->filtersLayout->setGeometry(QRect(0, 0, event->size().width(), event->size().height()-10));

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
        else
        if (obj == ui->centralWidget)
            this->on_resize_centralWidget (static_cast<QResizeEvent*>(event));
    }
    return QWidget::eventFilter(obj, event);
}

/**** /ui ****/

/**** actions ****/

void MainWindowFull::on_actionHelp_triggered()
{
    HelpWindow *win = new HelpWindow(this);
    win->show();
}

void MainWindowFull::on_actionQuit_triggered()
{
    application->quit();
}

void MainWindowFull::on_toolActionHelp_triggered()
{
    this->on_actionHelp_triggered();
}

/**** /actions ****/

/**** updateProjects ****/


void projectWidgetItemSetText(QWidget *__this, QTreeWidgetItem *widgetItem, QJsonObject item, RedmineItemTree *tree, int level) {
    (void)tree; (void)level;
    int item_id = item["id"].toInt();
    MainWindowFull *_this = reinterpret_cast<MainWindowFull *>(__this);

    widgetItem->setText(0,
        item["name"].toString()+
            " <span style='color: #808080'>("+
                QString::number(_this->issues_get_byProjectId(item_id).count())+
            ")</span>");

    return;
}

bool projectsFilter(QWidget *__this, QJsonObject item)
{
    MainWindowFull *_this = reinterpret_cast<MainWindowFull *>(__this);

    (void)_this; (void)item;

    return true;
}

void MainWindowFull::projects_display()
{
    this->issues_byProjectId.clear();
    foreach (const QJsonObject &issue, this->issues.get()) {
        int project_id = issue["project"].toObject()["id"].toInt();
        this->issues_byProjectId[project_id].append(issue);
    }

    this->projects.filter(reinterpret_cast<QWidget *>(this), projectsFilter);
    this->projects.display(this->ui->projects, reinterpret_cast<QWidget *>(this), projectWidgetItemSetText);
}

/**** /updateProjects ****/

/**** updateIssues ****/

void issuesWidgetItemSetText(QWidget *__this, QTreeWidgetItem *widgetItem, QJsonObject item, RedmineItemTree *tree, int level) {
    (void)__this; (void)tree; (void)level;
    //int item_id = item["id"].toInt();
    //MainWindowFull *_this = reinterpret_cast<MainWindowFull *>(__this);

    widgetItem->setText(0,
        item["subject"].toString());

    return;
}

bool issuesFilter(QWidget *__this, QJsonObject item)
{
    MainWindowFull *_this = reinterpret_cast<MainWindowFull *>(__this);

    if (_this->selected_projects_id.empty())
        return true;

    return _this->selected_projects_id.contains(item["project"].toObject()["id"].toInt());
}

void MainWindowFull::issues_display()
{
    qDebug("MainWindowFull::issues_display()");

    this->issues.filter(reinterpret_cast<QWidget *>(this), issuesFilter);
    this->issues.display(this->ui->issuesTree, reinterpret_cast<QWidget *>(this), issuesWidgetItemSetText);

    return;
}

/**** /updateIssues ****/

/**** SIGNALS ****/

void MainWindowFull::on_projects_itemSelectionChanged()
{
    this->selected_projects_id.clear();

    foreach (QTreeWidgetItem *selectedProjectItem, this->ui->projects->selectedItems()) {
        QJsonObject project = this->projects.get(selectedProjectItem);
        this->selected_projects_id.insert(project["id"].toInt(), true);
    }

    this->issues_display();

    return;
}

void MainWindowFull::on_issuesTree_itemSelectionChanged()
{

}

/**** /SIGNALS ****/
