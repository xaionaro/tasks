#include "redmineclass_time_entry.h"


RedmineClass_TimeEntry::RedmineClass_TimeEntry()
{
    this->redmine = NULL;
}

int RedmineClass_TimeEntry::setRedmine(Redmine *redmine)
{
    this->redmine = redmine;
}

RedmineClass_TimeEntry::RedmineClass_TimeEntry(Redmine *redmine)
{
    this->setRedmine(redmine);
}

int RedmineClass_TimeEntry::save() {
    if (this->redmine == NULL)
        return EHOSTUNREACH;

    QVariantMap timeEntries, timeEntry;
    RedmineClient::EMode mode = (this->id != 0 ? RedmineClient::EMode::PUT : RedmineClient::EMode::POST);

    if (this->id != 0)
        timeEntry["id"]   = QVariant(this->id);

    timeEntry["hours"]    = QVariant(this->hours);
    timeEntry["endtime"]  = QVariant(this->endtime);
    timeEntry["issue_id"] = QVariant(this->issueId);
    timeEntry["comment"]  = QVariant(this->comment);

    timeEntries["time_entry"] = timeEntry;

    this->redmine->request(mode, "time_entries", NULL, NULL, NULL, false, "", timeEntries);
    return 0;

}

int RedmineClass_TimeEntry::set(QDateTime timeFrom, QDateTime timeTo, int issueId, QString comment) {
    this->hours   = ((float)((time_t)(timeTo.toTime_t() - timeFrom.toTime_t()))) / 3600;
    this->endtime =  timeTo;
    this->issueId = issueId;
    this->comment = comment;

    if (issueId == 0) {
        QJsonValueRef defaultIssueIdObj = this->redmine->me()["default_issue_id"];
        if (!defaultIssueIdObj.toInt()) {
            qDebug("RedmineClass_TimeEntry::set(): Invalid issueId");
            return EINVAL;
        }
        issueId = defaultIssueIdObj.toInt();
    }

    return 0;
}

/*
int RedmineClass_TimeEntry::set(QJsonObject *json) {
    return 0;
}*/
