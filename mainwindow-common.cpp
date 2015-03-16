
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

