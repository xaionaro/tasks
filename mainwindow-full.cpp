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
#include "projectmemberswindow.h"

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
    connect(this->timerUpdateIssues, SIGNAL(timeout()),   this, SLOT(updateIssues()));
    this->timerUpdateIssues->start(60000);

    this->timerUpdateProjects = new QTimer(this);
    connect(this->timerUpdateProjects, SIGNAL(timeout()), this, SLOT(updateProjects()));
    this->timerUpdateProjects->start(60000);

    QDate date;
    int year_start = 2013;
    int year_cur   = qMax(2015, date.year());

    int year       = year_cur;
    while (year >= year_start)
        this->ui->issuesFilter_year->addItem(QString::number(year--));

    this->ui->projects->setSortingEnabled(true);
    this->ui->projects->sortByColumn(0, Qt::AscendingOrder);
    this->ui->projects->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->ui->projects->setContextMenuPolicy(Qt::CustomContextMenu);

    this->ui->issuesTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    this->ui->issuesTree->header()->setStretchLastSection(false);
    this->ui->issuesTree->setSortingEnabled(true);
    this->ui->issuesTree->sortByColumn(4, Qt::DescendingOrder);
    this->ui->issuesTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->ui->issuesTree->setContextMenuPolicy(Qt::CustomContextMenu);

    /*
    connect(this->ui->projects, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(projectsShowContextMenu(const QPoint&)));
    connect(this->ui->issuesTree, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(issuesShowContextMenu(const QPoint&)));
     */

    this->projectsDisplayRetryTimer.setSingleShot(true);
    connect(&this->projectsDisplayRetryTimer, SIGNAL(timeout()), this, SLOT(projects_display()));

    this->ui->issuesFilter_field_assigned_to->addItem("", 0);
    this->ui->issuesFilter_field_status     ->addItem("", 0);

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

    QHeaderView *issuesHeader = this->ui->issuesTree->header();

    this->ui->issuesFilter_field_name       ->resize(issuesHeader->sectionSize(0), 24);
    this->ui->issuesFilter_field_assigned_to->resize(issuesHeader->sectionSize(1), 24);
    this->ui->issuesFilter_field_due_date   ->resize(issuesHeader->sectionSize(2), 24);
    this->ui->issuesFilter_field_status     ->resize(issuesHeader->sectionSize(3), 24);
    this->ui->issuesFilter_field_updated_on ->resize(issuesHeader->sectionSize(4), 24);

    this->ui->issuesFilter_field_name       ->move(issuesHeader->sectionPosition(0), 0);
    this->ui->issuesFilter_field_assigned_to->move(issuesHeader->sectionPosition(1), 0);
    this->ui->issuesFilter_field_due_date   ->move(issuesHeader->sectionPosition(2), 0);
    this->ui->issuesFilter_field_status     ->move(issuesHeader->sectionPosition(3), 0);
    this->ui->issuesFilter_field_updated_on ->move(issuesHeader->sectionPosition(4), 0);

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
    this->ui->projects->resize(event->size().width(), event->size().height()-105);

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

    if (item["name"].toString().indexOf(_this->projectsFilter_namePart, Qt::CaseInsensitive) == -1)
        return false;

    return true;
}

void MainWindowFull::projects_display()
{
    if (!this->projectsDisplayMutex.tryLock()) {
        if (!this->projectsDisplayExceptionMutex.tryLock())
            return;

        this->projectsDisplayRetryTimer.start(100);

        this->projectsDisplayExceptionMutex.unlock();
        return;
    }

    this->issues_byProjectId.clear();
    foreach (const QJsonObject &issue, this->issues.get()) {

        if (this->issue_isFiltered(issue))
            continue;

        QJsonObject project    = issue["project"].toObject();
        int         project_id = project["id"].toInt();
        this->issues_byProjectId[project_id].append(issue);

        if (this->showProjectIssues_recursive) {
            QJsonObject parent;

            project = this->projects.get(project["id"].toInt());
            while (true) {
                int parent_id;

                parent    = this->projects.get(project["parent"].toObject()["id"].toInt());
                parent_id = parent["id"].toInt();

                if (parent_id == 0)
                    break;

                this->issues_byProjectId[parent_id].append(issue);
                project = parent;
            }
        }
    }

    this->projects.filter(reinterpret_cast<QWidget *>(this), projectsFilter);
    this->projects.display(this->ui->projects, reinterpret_cast<QWidget *>(this), projectWidgetItemSetText);

    this->projectsDisplayMutex.unlock();
}

/**** /updateProjects ****/

/**** updateIssues ****/

bool MainWindowFull::issue_isFiltered(QJsonObject issue)
{
    if (!this->showIssues_showClosed) {
        QJsonObject issue_status = issue["status"].toObject();
        bool        isClosed     = redmine->get_issue_status(issue_status["id"].toInt())["is_closed"].toBool();

        if (isClosed)
            return true;
    }

    if (this->issuesFilter_queryType != IFQT_ALL) {
        QJsonObject author   = issue["author"].toObject();
        QJsonObject assignee = issue["assigned_to"].toObject();
        QJsonObject me       = redmine->me();

        int author_id   = author["id"].toInt();
        int assignee_id = assignee["id"].toInt();
        int me_id       = me["id"].toInt();

        switch (this->issuesFilter_queryType) {
            case IFQT_TOME:
                if (!(assignee_id == me_id))
                    return true;
                break;
            case IFQT_FROMME:
                if (!(assignee_id != me_id && author_id == me_id))
                    return true;
                break;
            case IFQT_FOLLOWED:
                if (!(assignee_id != me_id && author_id != me_id))
                    return true;
                break;
            case IFQT_ALL:
                qFatal("MainWindowFull::issue_isFiltered(): this shouldn't happend");
        }
    }

    if (this->issuesFilter_field_assignee_id != 0)
        if (issue["assigned_to"].toObject()["id"].toInt() != this->issuesFilter_field_assignee_id)
            return true;

    if (this->issuesFilter_field_status_id != 0)
        if (issue["status"].toObject()["id"].toInt() != this->issuesFilter_field_status_id)
            return true;

    if (this->issuesFilter_field_subjectPart.size())
        if (issue["subject"].toString().indexOf(this->issuesFilter_field_subjectPart, Qt::CaseInsensitive) == -1)
            return true;

    return false;
}

void issuesWidgetItemSetText(QWidget *__this, QTreeWidgetItem *widgetItem, QJsonObject issue, RedmineItemTree *tree, int level) {
    (void)__this; (void)tree; (void)level;
    //int item_id = item["id"].toInt();
    //MainWindowFull *_this = reinterpret_cast<MainWindowFull *>(__this);

    QJsonObject  issue_status = issue["status"].toObject();
//    bool         isClosed     = redmine->get_issue_status(issue_status["id"].toInt())["is_closed"].toBool();


    widgetItem->setText(0, issue["subject"].toString());
    widgetItem->setText(1, issue["assigned_to"].toObject()["name"].toString());

    QString due_date_str = issue["due_date"].toString();
    QDateTime now, date;
    now  = QDateTime::currentDateTime();
    date = QDateTime::fromString(due_date_str, "yyyy-MM-dd");

    widgetItem->setText(2, due_date_str);
    widgetItem->setText(3, issue_status["name"].toString());

    QDateTime updated_on = redmine->parseDateTime(issue["updated_on"]);
    widgetItem->setText(4, updated_on.toString("yyyy'-'MM'-'dd HH':'MM"));

    return;
}

bool issuesFilter(QWidget *__this, QJsonObject item)
{
    MainWindowFull *_this = reinterpret_cast<MainWindowFull *>(__this);
    int             project_id;

    if (_this->issue_isFiltered(item))
        return false;

    if (_this->selected_projects_id.empty())
        return true;

    project_id = item["project"].toObject()["id"].toInt();

    if (_this->selected_projects_id.contains(project_id))
        return true;

    if (!_this->showProjectIssues_recursive)
        return false;

    foreach (int selected_project_id, _this->selected_projects_id)
        if (_this->projects.isDescendant(project_id, selected_project_id))
            return true;

    return false;
}

void MainWindowFull::setIssuesFilterItems(QComboBox *box, QHash<int,QJsonObject> table_old, QHash<int,QJsonObject> table, QString keyname) {
    QHash<int, int> ids_old;

    foreach (const int &item_id, table_old.keys())
        ids_old.insert(item_id, item_id);

    /*
     * Adding new items
     */

    foreach (const int &item_id, table.keys()) {
        if (ids_old.contains(item_id))
            ids_old.remove(item_id);
        else
             box->addItem(table[item_id][keyname].toString(), item_id);
    }

    /*
     * Removing old items
     */

    int idx = box->count();
    while (--idx > 0) {
        int item_id;

        item_id = box->itemData(idx).toInt();

        if (ids_old.contains(item_id))
            box->removeItem(idx);
    }

    /*
     * Sorting
     */

    box->model()->sort(0);

    return;
}


void MainWindowFull::issues_display()
{
    qDebug("MainWindowFull::issues_display()");

    this->issues.filter(reinterpret_cast<QWidget *>(this), issuesFilter);
    this->issues.display(this->ui->issuesTree, reinterpret_cast<QWidget *>(this), issuesWidgetItemSetText);

    QHash<int, QJsonObject> issuesFiltered_statuses_old  = this->issuesFiltered_statuses;
    QHash<int, QJsonObject> issuesFiltered_assignees_old = this->issuesFiltered_assignees;
    //QHash<int, QJsonObject> issuesFiltered_author_old    = this->issuesFiltered_author;

    this->issuesFiltered_statuses.clear();
    this->issuesFiltered_assignees.clear();
    this->issuesFiltered_authors.clear();
    foreach (const QJsonObject &issue, this->issues.filtered.get())
    {
        QJsonObject status   = issue["status"].toObject();
        QJsonObject assignee = issue["assigned_to"].toObject();
        QJsonObject author   = issue["author"].toObject();

        int status_id   = status  ["id"].toInt();
        int assignee_id = assignee["id"].toInt();
        int author_id   = author  ["id"].toInt();

        this->issuesFiltered_authors.insert(author_id, author);

        if (status_id   != 0)
            this->issuesFiltered_statuses. insert(status_id,   status);
        if (assignee_id != 0)
            this->issuesFiltered_assignees.insert(assignee_id, assignee);
    }

    this->setIssuesFilterItems(this->ui->issuesFilter_field_assigned_to,
                               issuesFiltered_assignees_old,
                               this->issuesFiltered_assignees,
                               "name");

    this->setIssuesFilterItems(this->ui->issuesFilter_field_status,
                               issuesFiltered_statuses_old,
                               this->issuesFiltered_statuses,
                               "name");
    this->projects_display();

    return;
}

/**** /updateIssues ****/

/**** issue_display ****/

void MainWindowFull::issue_display_dateField(QString field_name, QString field_value)
{
    qDebug("issue_display_dateField: %s", field_name.toUtf8().data());
}

void MainWindowFull::issue_display_dateTimeField(QString field_name, QString field_value)
{
    qDebug("issue_display_dateTimeField: %s", field_name.toUtf8().data());
}

void MainWindowFull::issue_display_stringField(QString field_name, QString field_value)
{
    qDebug("issue_display_string: %s", field_name.toUtf8().data());
}

void MainWindowFull::issue_display_enumField(QString field_name, int field_value_id)
{
    qDebug("issue_display_enumField: %s", field_name.toUtf8().data());
}

void MainWindowFull::issue_clear()
{

}

void MainWindowFull::issue_display(int issue_id)
{
    QJsonObject issue;
    //QList<QString> std_simplefields << "description" << "start_date" << "due_date" << "created_on" << "updated_on";
    //QList<QString> std_arrayfields  << "project" << "tracker" << "status" << "priority" << "author" << "assigned_to";

    if (issue_id == 0) {
        this->ui->issueTitle->setText("");
        return;
    }

    issue = this->issues.get(issue_id);
    this->issue_clear();

    this->ui->issueTitle->setText("["+issue["tracker"].toObject()["name"].toString()+" #"+QString::number(issue["id"].toInt())+"] "+issue["subject"].toString());

     QJsonObject::iterator iterator  = issue.begin();
     QJsonObject::iterator issue_end = issue.end();
     while (iterator != issue_end)
     {
         QString    key   = iterator.key();
         QJsonValue value = iterator.value();

         iterator++;

         if (key == "custom_fields")
             continue;

         if (value.isString()) { // String, Date or DateTime
             /*
              * Checking if it's a Date
              */

             if (key.endsWith("_date")) {
                 this->issue_display_dateField(key, value.toString());
                 continue;
             }

             /*
              * Checking if it's a DateTime
              */

             if (key.endsWith("_on")) {
                 this->issue_display_dateTimeField(key, value.toString());
                 continue;
             }

             /*
              * Otherwise it's a String
              */

             this->issue_display_stringField(key, value.toString());

         } else
         if (value.isObject()) { // Enum
             this->issue_display_enumField  (key, value.toObject()["id"].toInt());
         }
     }

}

/**** /issue_display ****/

/**** SIGNALS ****/

void MainWindowFull::on_projects_itemSelectionChanged()
{
    this->selected_projects_id.clear();

    foreach (QTreeWidgetItem *selectedProjectItem, this->ui->projects->selectedItems()) {
        QJsonObject project    = this->projects.get(selectedProjectItem);
        int         project_id = project["id"].toInt();
        //qDebug("selected project: %i", project["id"].toInt());
        this->selected_projects_id.insert(project_id, project_id);
    }

    this->issues_display();

    return;
}

void MainWindowFull::on_issuesTree_itemSelectionChanged()
{
    this->selected_issues_id.clear();

    foreach (QTreeWidgetItem *selectedIssueItem, this->ui->issuesTree->selectedItems()) {
        QJsonObject issue    = this->issues.get(selectedIssueItem);
        int         issue_id = issue["id"].toInt();
        this->selected_issues_id.insert(issue_id, issue_id);
    }

    if (this->selected_issues_id.count() == 1)
        this->issue_display(this->selected_issues_id.keys().first());
    else
        this->issue_display(0);
}


void MainWindowFull::on_projectsRadio_recursive_off_toggled(bool checked)
{
    if (this->ui->projectsRadio_recursive_on->isChecked() == checked)
        this->ui->projectsRadio_recursive_on->setChecked(!checked);
}

void MainWindowFull::on_projectsRadio_recursive_on_toggled(bool checked)
{
    if (this->ui->projectsRadio_recursive_off->isChecked() == checked)
        this->ui->projectsRadio_recursive_off->setChecked(!checked);

    if (this->showProjectIssues_recursive != checked) {
        this->showProjectIssues_recursive = checked;
        this->issues_display();
    }
}

void MainWindowFull::on_issuesFilter_showClosed_yes_toggled(bool checked)
{
    if (this->ui->issuesFilter_showClosed_no->isChecked() == checked)
        this->ui->issuesFilter_showClosed_no->setChecked(!checked);

    if (this->showIssues_showClosed != checked) {
        this->showIssues_showClosed = checked;
        this->issues_display();
    }
}

void MainWindowFull::on_issuesFilter_showClosed_no_toggled(bool checked)
{
    if (this->ui->issuesFilter_showClosed_yes->isChecked() == checked)
        this->ui->issuesFilter_showClosed_yes->setChecked(!checked);
}

void MainWindowFull::on_issuesFilter_queryType_all_toggled(bool checked)
{
    if (
            this->ui->issuesFilter_queryType_all->isChecked() == false &&
            this->ui->issuesFilter_queryType_toMe->isChecked() == false &&
            this->ui->issuesFilter_queryType_fromMe->isChecked() == false &&
            this->ui->issuesFilter_queryType_followed->isChecked() == false
       )
    {
        this->ui->issuesFilter_queryType_all->setChecked(true);
    }

    if (checked == true) {
        this->ui->issuesFilter_queryType_followed->setChecked(false);
        this->ui->issuesFilter_queryType_toMe->setChecked(false);
        this->ui->issuesFilter_queryType_fromMe->setChecked(false);
    }

    this->issuesFilter_queryType = IFQT_ALL;

    this->issues_display();

    return;
}

void MainWindowFull::on_issuesFilter_queryType_followed_toggled(bool checked)
{
    if (
            this->ui->issuesFilter_queryType_all->isChecked() == false &&
            this->ui->issuesFilter_queryType_toMe->isChecked() == false &&
            this->ui->issuesFilter_queryType_fromMe->isChecked() == false &&
            this->ui->issuesFilter_queryType_followed->isChecked() == false
       )
    {
        this->ui->issuesFilter_queryType_followed->setChecked(true);
    }

    if (checked == true) {
        this->ui->issuesFilter_queryType_all->setChecked(false);
        this->ui->issuesFilter_queryType_toMe->setChecked(false);
        this->ui->issuesFilter_queryType_fromMe->setChecked(false);
    }

    this->issuesFilter_queryType = IFQT_FOLLOWED;

    this->issues_display();

    return;
}

void MainWindowFull::on_issuesFilter_queryType_fromMe_toggled(bool checked)
{
    if (
            this->ui->issuesFilter_queryType_all->isChecked() == false &&
            this->ui->issuesFilter_queryType_toMe->isChecked() == false &&
            this->ui->issuesFilter_queryType_fromMe->isChecked() == false &&
            this->ui->issuesFilter_queryType_followed->isChecked() == false
       )
    {
        this->ui->issuesFilter_queryType_fromMe->setChecked(true);
    }


    if (checked == true) {
        this->ui->issuesFilter_queryType_all->setChecked(false);
        this->ui->issuesFilter_queryType_toMe->setChecked(false);
        this->ui->issuesFilter_queryType_followed->setChecked(false);
    }

    this->issuesFilter_queryType = IFQT_FROMME;

    this->issues_display();

    return;
}

void MainWindowFull::on_issuesFilter_queryType_toMe_toggled(bool checked)
{
    if (
            this->ui->issuesFilter_queryType_all->isChecked() == false &&
            this->ui->issuesFilter_queryType_toMe->isChecked() == false &&
            this->ui->issuesFilter_queryType_fromMe->isChecked() == false &&
            this->ui->issuesFilter_queryType_followed->isChecked() == false
       )
    {
        this->ui->issuesFilter_queryType_toMe->setChecked(true);
    }

    if (checked == true) {
        this->ui->issuesFilter_queryType_all->setChecked(false);
        this->ui->issuesFilter_queryType_fromMe->setChecked(false);
        this->ui->issuesFilter_queryType_followed->setChecked(false);
    }

    this->issuesFilter_queryType = IFQT_TOME;

    this->issues_display();

    return;
}

/**** /SIGNALS ****/

void MainWindowFull::on_issuesFilter_year_currentIndexChanged(int index)
{
    this->issuesFilter_yearIdx = index;
    this->updateIssues();
}

void MainWindowFull::on_projectFilter_field_name_textChanged(const QString &arg1)
{
    this->projectsFilter_namePart = arg1;
    this->issues_display();
}

void MainWindowFull::projectsShowContextMenu(const QPoint &pos)
{
    QList<int> project_ids = this->selected_projects_id.keys();
    QPoint     globalPos   = this->ui->projects->mapToGlobal(pos);

    QMenu contextMenu;
    QAction *membersItem = contextMenu.addAction("Участники");

    QAction *selectedItem = contextMenu.exec(globalPos);

    qDebug("%p %p", membersItem, selectedItem);
    if (selectedItem == membersItem) {
        ProjectMembersWindow *membersWindow = new ProjectMembersWindow(this, project_ids);
        membersWindow->show();
    } else
    {}

    return;
}

void MainWindowFull::issuesShowContextMenu(const QPoint &pos)
{
    return;
}


void MainWindowFull::on_issuesFilter_field_name_textChanged(const QString &arg1)
{
    this->issuesFilter_field_subjectPart = arg1;
    this->issues_display();
}

void MainWindowFull::on_issuesFilter_field_assigned_to_currentIndexChanged(int index)
{
    int assigned_to_id = this->ui->issuesFilter_field_assigned_to->itemData(index).toInt();
    if (this->issuesFilter_field_assignee_id != assigned_to_id) {
        this->issuesFilter_field_assignee_id  = assigned_to_id;
        this->issues_display();
    }
}

void MainWindowFull::on_issuesFilter_field_status_currentIndexChanged(int index)
{
    int status_id = this->ui->issuesFilter_field_status->itemData(index).toInt();
    if (this->issuesFilter_field_status_id != status_id) {
        this->issuesFilter_field_status_id  = status_id;
        this->issues_display();
    }
}
