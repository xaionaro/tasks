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
    //QDateTime issue_updatedOn_a = redmine->parseDateTime(issue_a["updated_on"]);
    //QDateTime issue_updatedOn_b = redmine->parseDateTime(issue_b["updated_on"]);

    //qDebug("%s < %s == %i", issue_a["updated_on"].toString().toUtf8().data(), issue_b["updated_on"].toString().toUtf8().data(), issue_updatedOn_a < issue_updatedOn_b);

    return issue_updatedOn_a < issue_updatedOn_b;
}
bool issueCmpFunct_updatedOn_gt(const QJsonObject &issue_a, const QJsonObject &issue_b) {
    QString issue_updatedOn_a = issue_a["updated_on"].toString();
    QString issue_updatedOn_b = issue_b["updated_on"].toString();

    return issue_updatedOn_a > issue_updatedOn_b;
}


MainWindowCommon::MainWindowCommon(QWidget *parent) :
    QMainWindow(parent)
{
    this->sortFunctMap.insert(SORT_UNDEFINED,           NULL);

    this->sortFunctMap.insert(SORT_STATUS_POS_ASC,      issueCmpFunct_statusPosition_lt);
    this->sortFunctMap.insert(SORT_STATUS_POS_DESC,     issueCmpFunct_statusPosition_gt);

    this->sortFunctMap.insert(SORT_UPDATED_ON_ASC,      issueCmpFunct_updatedOn_lt);
    this->sortFunctMap.insert(SORT_UPDATED_ON_DESC,     issueCmpFunct_updatedOn_gt);

    this->sortFunctMap.insert(SORT_STATUS_ISCLOSED_ASC, issueCmpFunct_statusIsClosed_lt);

    memset(this->sortColumn, 0, sizeof(this->sortColumn));
    return;
}
