#ifndef REDMINECLASS_TIME_ENTRY_H
#define REDMINECLASS_TIME_ENTRY_H

#include <QObject>
#include <QTimer>
#include "redmine.h"

class RedmineClass_TimeEntry : public QObject
{
	Q_OBJECT
	CALLBACK_DISPATCHER ( Redmine, RedmineClass_TimeEntry, this )

private:
	Redmine *redmine;
	int id;
	int issueId;
	int projectId;
	float hours;
	QString comment;
	QDateTime endtime;
	int activityId;
	void init();
	QTimer saveTimer;
	QNetworkReply *saveReply;

signals:
	void on_saveSuccess();
	void on_saveTimeout();
	void on_saveFailure ( QNetworkReply *reply );

private slots:
	void saveTimeout();

public:
	int save();
	//int set(QJsonObject *json);
	int set ( QDateTime timeFrom, QDateTime timeTo, int projectId, int issueId, QString comment, int activityId );
	int setIssueId ( int issueId );
	int getIssueId();
	int setProjectId ( int projectId );
	int setRedmine ( Redmine *redmine );
	void saveCallback(QNetworkReply *reply, QJsonDocument *timeEntry_doc, void *_null );

	RedmineClass_TimeEntry();
	RedmineClass_TimeEntry ( Redmine *redmine );
	~RedmineClass_TimeEntry();
};

#endif // REDMINECLASS_TIME_ENTRY_H
