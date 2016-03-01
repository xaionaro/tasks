#include "logtimewindow.h"
#include "ui_logtimewindow.h"

#include "redmineclass_time_entry.h"

LogTimeWindow::LogTimeWindow(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::LogTimeWindow)
{
    this->ui->setupUi(this);
    this->ui->untilInput->setTime(QTime::currentTime());
    this->ui->sinceInput->setTime(QTime::fromString("09:00", "hh':'mm"));

    this->setWindowTitle("Система «Задачи» НИЯУ МИФИ: Учёт времени");
    connect(redmine, SIGNAL(callback_call      (void*,callback_t,QNetworkReply*,QJsonDocument*,void*)),
            this,    SLOT(  callback_dispatcher(void*,callback_t,QNetworkReply*,QJsonDocument*,void*)) );

    this->updateLastLogTime();
    this->updateIssues();
    this->updateProjects();
}

LogTimeWindow::~LogTimeWindow()
{
    delete ui;
}

void LogTimeWindow::get_time_entries_callback(QNetworkReply *reply, QJsonDocument *json, void *arg) {
    (void)reply; (void)arg;

    // Using "to" field of the last timelog entry as a "since"/"from" value for a new entry

    QJsonArray time_entries = json->object()["time_entries"].toArray();

    qDebug("get_time_entries_callback: time_entries.count() == %i", time_entries.count());

    if (time_entries.count() == 0)
        return;

    QString time_entry_until;
    time_entry_until = time_entries.at(0).toObject()["to"].toString();

    QDateTime cur   = QDateTime::currentDateTime();
    QDateTime until = QDateTime::fromString(time_entry_until, Qt::ISODate);

    qDebug("get_time_entries_callback: time_entry_until == \"%s\"", time_entry_until.toStdString().c_str());

    if (cur.date() > until.date()) {
        qDebug("get_time_entries_callback: cur.date() > until.date(): %s > %s", cur.toString(Qt::ISODate).toStdString().c_str(), until.toString(Qt::ISODate).toStdString().c_str());
        return;
    }

    this->ui->sinceInput->setTime(until.time());

    return;
}

int LogTimeWindow::updateLastLogTime() {
    qDebug("updateLastLogTime()");
    redmine->get_time_entries((Redmine::callback_t)&LogTimeWindow::get_time_entries_callback, this, false, "user_id=me&limit=1");
    return 0;
}

void LogTimeWindow::on_cancel_clicked()
{
    delete this;
}

void LogTimeWindow::on_accept_clicked()
{
    RedmineClass_TimeEntry timeEntry;

    timeEntry.setRedmine(redmine);
    timeEntry.set(this->ui->sinceInput->dateTime(), this->ui->untilInput->dateTime(), 0, this->ui->comment->toPlainText());
    timeEntry.save();

    delete this;
}


/**** updateProjects ****/

void projectsAddItemFunct(QComboBox *projects, QJsonObject project)
{
    int projectId = project["id"].toInt();
    QString projectIdentifier = project["identifier"].toString();
    QString projectName       = project["name"].toString();
    QString caption = QString("%1: %2").arg(projectIdentifier).arg(projectName);
    projects->addItem(caption, projectId);

    return;
}


bool LogTimeWindow_projectsFilter(QWidget *__this, QJsonObject item)
{
    LogTimeWindow *_this = reinterpret_cast<LogTimeWindow *>(__this);

    (void)_this; (void)item;

    //if (item["name"].toString().indexOf(_this->projectsFilter_namePart, Qt::CaseInsensitive) == -1)
    //    return false;

    return true;
}


void LogTimeWindow::projects_display()
{
    qDebug("LogTimeWindow::projects_display()");

    if (!this->projectsDisplayMutex.tryLock()) {
        if (!this->projectsDisplayExceptionMutex.tryLock())
            return;

        this->projectsDisplayRetryTimer.start(100);

        this->projectsDisplayExceptionMutex.unlock();
        return;
    }

    this->issues_byProjectId.clear();
    foreach (const QJsonObject &issue, this->issues.get()) {

/*        if (this->issue_isFiltered(issue))
            continue;*/

        QJsonObject project    = issue["project"].toObject();
        int         project_id = project["id"].toInt();
        this->issues_byProjectId[project_id].append(issue);
    }

    this->projects.filter(reinterpret_cast<QWidget *>(this), LogTimeWindow_projectsFilter);
    this->projects.display(this->ui->project, reinterpret_cast<QWidget *>(this), projectsAddItemFunct);

    this->projectsDisplayMutex.unlock();
}

void LogTimeWindow::get_projects_callback(QNetworkReply *reply, QJsonDocument *json, void *arg) {
    (void)reply; (void)arg;

    this->updateProjectsMutex.lock(); // is not a thread-safe function, locking

    QJsonObject answer   = json->object();
    QJsonArray  projects = answer["projects"].toArray();

    this->projects.set(projects);

    this->projects_display();

    this->updateProjectsMutex.unlock();
    return;
}

QList<QJsonObject> LogTimeWindow::issues_get_byProjectId(int project_id)
{
    return this->issues_byProjectId[project_id];
}

int LogTimeWindow::updateProjects() {
    redmine->get_projects(this, (Redmine::callback_t)&LogTimeWindow::get_projects_callback, NULL);
    return 0;
}

/**** /updateProjects ****/

/**** updateIssues ****/

void LogTimeWindow_issuesWidgetItemSetText(QWidget *__this, QTreeWidgetItem *widgetItem, QJsonObject issue, RedmineItemTree *tree, int level) {
    (void)__this; (void)tree; (void)level;

    widgetItem->setText(0, QString::number(issue["id"].toInt()));
    widgetItem->setText(1, issue["subject"].toString());

    return;
}

bool LogTimeWindow_issuesFilter(QWidget *__this, QJsonObject item)
{
    LogTimeWindow  *_this = reinterpret_cast<LogTimeWindow *>(__this);
    int             project_id;

    //if (_this->selected_projects_id.empty())
        return true;
/*
    project_id = item["project"].toObject()["id"].toInt();

    if (_this->selected_projects_id.contains(project_id))
        return true;

    if (!_this->showProjectIssues_recursive)
        return false;

    foreach (int selected_project_id, _this->selected_projects_id)
        if (_this->projects.isDescendant(project_id, selected_project_id))
            return true;
*/
    return false;
}

void LogTimeWindow::setIssuesFilterItems(QComboBox *box, QHash<int,QJsonObject> table_old, QHash<int,QJsonObject> table, QString keyname) {
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


void LogTimeWindow::issues_display()
{
    qDebug("LogTimeWindow::issues_display()");
    this->issues.filter(reinterpret_cast<QWidget *>(this), LogTimeWindow_issuesFilter);
    this->issues.display(this->ui->issue, reinterpret_cast<QWidget *>(this), LogTimeWindow_issuesWidgetItemSetText);
    return;
}

void LogTimeWindow::get_issues_callback(QNetworkReply *reply, QJsonDocument *json, void *arg) {
    (void)reply; (void)arg;

    this->updateIssuesMutex.lock(); // is not a thread-safe function, locking

    QJsonObject answer   = json->object();
    QJsonArray  issues = answer["issues"].toArray();

    this->issues.set(issues);

    this->issues_display();

    this->updateIssuesMutex.unlock();

    return;
}

int LogTimeWindow::updateIssues() {
    redmine->get_issues(this, (Redmine::callback_t)&LogTimeWindow::get_issues_callback, this);
    return 0;
}

/**** /updateIssues ****/
