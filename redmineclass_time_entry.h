#ifndef REDMINECLASS_TIME_ENTRY_H
#define REDMINECLASS_TIME_ENTRY_H

#include "redmine.h"

class RedmineClass_TimeEntry
{
private:
    Redmine *redmine;
    int id;
    int issueId;
    float hours;
    QString comment;
    QDateTime endtime;

public:
    int save();
    //int set(QJsonObject *json);
    int set(QDateTime timeFrom, QDateTime timeTo, int issueId, QString comment);
    int setRedmine(Redmine *redmine);

    RedmineClass_TimeEntry();
    RedmineClass_TimeEntry(Redmine *redmine);
};

#endif // REDMINECLASS_TIME_ENTRY_H
