#include "redmineclass_time_entry.h"

#include "errno.h"

RedmineClass_TimeEntry::RedmineClass_TimeEntry()
{
    this->id      = 0;
    this->redmine = NULL;
    return;
}

int RedmineClass_TimeEntry::setRedmine(Redmine *redmine)
{
    this->redmine = redmine;
    return 0;
}

RedmineClass_TimeEntry::RedmineClass_TimeEntry(Redmine *redmine)
{
    this->id = 0;
    this->setRedmine(redmine);
    return;
}

int RedmineClass_TimeEntry::setIssueId(int issueId)
{
    this->issueId = issueId;
    return 0;
}

int RedmineClass_TimeEntry::getIssueId()
{
    return this->issueId;
}

int RedmineClass_TimeEntry::save() {
    if (this->redmine == NULL)
        return EHOSTUNREACH;

    QVariantMap timeEntries, timeEntry;
    QString uri;
    RedmineClient::EMode mode = (this->id != 0 ? RedmineClient::EMode::PUT : RedmineClient::EMode::POST);

    if (this->id != 0) {
        uri = "time_entries/"+QString(this->id);
        timeEntry["id"]    = QVariant(this->id);
    } else {
        uri = "time_entries";
    }

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
    if (issueId != -1)
        this->issueId = issueId;
    this->comment = comment;

    if (issueId == 0) {
        qDebug("RedmineClass_TimeEntry::set(): issueId is not set. Trying the default.");
        QJsonValueRef defaultIssueIdObj = this->redmine->me()["default_issue_id"];
        if (!defaultIssueIdObj.toInt()) {
            qDebug("RedmineClass_TimeEntry::set(): The default issueId is not set, too. ERROR.");
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
