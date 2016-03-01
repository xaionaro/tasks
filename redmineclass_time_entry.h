#ifndef REDMINECLASS_TIME_ENTRY_H
#define REDMINECLASS_TIME_ENTRY_H

#include "redmine.h"

class RedmineClass_TimeEntry
{
private:
	Redmine *redmine;
	int id;
	int issueId;
	int projectId;
	float hours;
	QString comment;
	QDateTime endtime;
	int activityId;

public:
	int save();
	//int set(QJsonObject *json);
	int set ( QDateTime timeFrom, QDateTime timeTo, int projectId, int issueId, QString comment, int activityId );
	int setIssueId ( int issueId );
	int getIssueId();
	int setProjectId ( int projectId );
	int setRedmine ( Redmine *redmine );

	RedmineClass_TimeEntry();
	RedmineClass_TimeEntry ( Redmine *redmine );
};

#endif // REDMINECLASS_TIME_ENTRY_H
