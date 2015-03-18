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

#include <QSettings>

#include "mainwindow-common.h"

bool issueCmpFunct_statusIsClosed_lt(const QJsonObject &issue_a, const QJsonObject &issue_b)
{
    int issue_statusIsClosed_a = redmine->get_issue_status(issue_a["status"].toObject()["id"].toInt())["is_closed"].toBool();
    int issue_statusIsClosed_b = redmine->get_issue_status(issue_b["status"].toObject()["id"].toInt())["is_closed"].toBool();

    return issue_statusIsClosed_a < issue_statusIsClosed_b;
}


bool issueCmpFunct_statusPosition_lt(const QJsonObject &issue_a, const QJsonObject &issue_b)
{
    int issue_statusPosition_a = redmine->get_issue_status(issue_a["status"].toObject()["id"].toInt())["position"].toInt();
    int issue_statusPosition_b = redmine->get_issue_status(issue_b["status"].toObject()["id"].toInt())["position"].toInt();

    return issue_statusPosition_a < issue_statusPosition_b;
}
bool issueCmpFunct_statusPosition_gt(const QJsonObject &issue_a, const QJsonObject &issue_b) {
    int issue_statusPosition_a = redmine->get_issue_status(issue_a["status"].toObject()["id"].toInt())["position"].toInt();
    int issue_statusPosition_b = redmine->get_issue_status(issue_b["status"].toObject()["id"].toInt())["position"].toInt();

    return issue_statusPosition_a > issue_statusPosition_b;
}

bool issueCmpFunct_updatedOn_lt(const QJsonObject &issue_a, const QJsonObject &issue_b)
{
    QString issue_updatedOn_a = issue_a["updated_on"].toString();
    QString issue_updatedOn_b = issue_b["updated_on"].toString();

    return issue_updatedOn_a < issue_updatedOn_b;
}
bool issueCmpFunct_updatedOn_gt(const QJsonObject &issue_a, const QJsonObject &issue_b) {
    QString issue_updatedOn_a = issue_a["updated_on"].toString();
    QString issue_updatedOn_b = issue_b["updated_on"].toString();

    return issue_updatedOn_a > issue_updatedOn_b;
}

bool issueCmpFunct_name_lt(const QJsonObject &issue_a, const QJsonObject &issue_b)
{
    QString issue_name_a = issue_a["name"].toString();
    QString issue_name_b = issue_b["name"].toString();

    return issue_name_a < issue_name_b;
}
bool issueCmpFunct_name_gt(const QJsonObject &issue_a, const QJsonObject &issue_b)
{
    QString issue_name_a = issue_a["name"].toString();
    QString issue_name_b = issue_b["name"].toString();

    return issue_name_a > issue_name_b;
}

bool issueCmpFunct_assignee_lt(const QJsonObject &issue_a, const QJsonObject &issue_b)
{
    QString issue_assignee_a = issue_a["assigned_to"].toObject()["name"].toString();
    QString issue_assignee_b = issue_b["assigned_to"].toObject()["name"].toString();

    return issue_assignee_a < issue_assignee_b;
}
bool issueCmpFunct_assignee_gt(const QJsonObject &issue_a, const QJsonObject &issue_b)
{
    QString issue_assignee_a = issue_a["assigned_to"].toObject()["name"].toString();
    QString issue_assignee_b = issue_b["assigned_to"].toObject()["name"].toString();

    return issue_assignee_a > issue_assignee_b;
}

bool issueCmpFunct_dueTo_lt(const QJsonObject &issue_a, const QJsonObject &issue_b)
{
    QString issue_dueTo_a = issue_a["due_date"].toString();
    QString issue_dueTo_b = issue_b["due_date"].toString();

    return issue_dueTo_a < issue_dueTo_b;
}
bool issueCmpFunct_dueTo_gt(const QJsonObject &issue_a, const QJsonObject &issue_b)
{
    QString issue_dueTo_a = issue_a["due_date"].toString();
    QString issue_dueTo_b = issue_b["due_date"].toString();

    return issue_dueTo_a > issue_dueTo_b;
}

MainWindowCommon::MainWindowCommon(QWidget *parent) :
    QMainWindow(parent)
{
    QSettings qsettings(settings.settingsFilePath, QSettings::IniFormat);

    this->sortFunctMap.insert(SORT_UNDEFINED,           NULL);

    this->sortFunctMap.insert(SORT_NAME_ASC,            issueCmpFunct_name_lt);
    this->sortFunctMap.insert(SORT_NAME_DESC,           issueCmpFunct_name_gt);

    this->sortFunctMap.insert(SORT_ASSIGNEE_ASC,        issueCmpFunct_assignee_lt);
    this->sortFunctMap.insert(SORT_ASSIGNEE_DESC,       issueCmpFunct_assignee_gt);

    this->sortFunctMap.insert(SORT_DUE_TO_ASC,          issueCmpFunct_dueTo_lt);
    this->sortFunctMap.insert(SORT_DUE_TO_DESC,         issueCmpFunct_dueTo_gt);

    this->sortFunctMap.insert(SORT_STATUS_POS_ASC,      issueCmpFunct_statusPosition_lt);
    this->sortFunctMap.insert(SORT_STATUS_POS_DESC,     issueCmpFunct_statusPosition_gt);

    this->sortFunctMap.insert(SORT_UPDATED_ON_ASC,      issueCmpFunct_updatedOn_lt);
    this->sortFunctMap.insert(SORT_UPDATED_ON_DESC,     issueCmpFunct_updatedOn_gt);

    this->sortFunctMap.insert(SORT_STATUS_ISCLOSED_ASC, issueCmpFunct_statusIsClosed_lt);

    memset(this->sortColumn, 0, sizeof(this->sortColumn));

    this->sortColumn[0] = (MainWindowCommon::ESortColumn)qsettings.value("sortcode"  ).toInt();
    this->sortLogicalIndex =                             qsettings.value("sortcolumn").toInt();
    this->sortOrder        =              (Qt::SortOrder)qsettings.value("sortorder" ).toInt();

    return;
}


MainWindowCommon::~MainWindowCommon()
{
    QSettings qsettings(settings.settingsFilePath, QSettings::IniFormat);
    qsettings.setValue("sortcode",   (int)this->sortColumn[0]);
    qsettings.setValue("sortcolumn", (int)this->sortLogicalIndex);
    qsettings.setValue("sortorder",  (int)this->sortOrder);
    return;
}

/**** updateProjects ****/

void MainWindowCommon::projects_clear()
{
    this->projects_list.clear();
}

void MainWindowCommon::project_add(QJsonObject project_json)
{
    this->projects_list.append(project_json);
}

void MainWindowCommon::projects_display()
{
    qFatal("projects_display() is not re-implemented by the derivative object");
}

QList<QJsonObject> MainWindowCommon::projects_get()
{
    return this->projects_list;
}

void MainWindowCommon::get_projects_callback(QNetworkReply *reply, QJsonDocument *json, void *arg) {
    (void)reply; (void)arg;

    QJsonObject answer   = json->object();
    QJsonArray  projects = answer["projects"].toArray();
    QList<QJsonObject> projects_list;

    /* refilling this->projects_list */

    this->projects_clear();

    foreach (const QJsonValue &project_val, projects)
        this->project_add(project_val.toObject());

    /* rebuilding this->projects_hierarchy */

    projects_list = this->projects_get();

    this->projects_hierarchy.clear();

    foreach (const QJsonObject &project, projects_list) {
        int parent_id;

        if (project.contains("parent"))
            parent_id = project["parent"].toObject()["id"].toInt();
        else
            parent_id = 0;

        this->projects_hierarchy[parent_id].append(project);
    }

    /* calling the display function */

    this->projects_display();
    return;
}

QList<QJsonObject> MainWindowCommon::projects_hierarchy_getchildren(int project_id)
{
    return this->projects_hierarchy[project_id];
}

int MainWindowCommon::updateProjects() {
    redmine->get_projects((Redmine::callback_t)&MainWindowCommon::get_projects_callback, this);
    return 0;
}

/**** /updateProjects ****/
