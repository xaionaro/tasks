#ifndef COMMON_H
#define COMMON_H

#include <QApplication>

#include "redmine.h"
#define SERVER_URL "https://tasks.mephi.ru"

extern Redmine      *redmine;
extern QApplication *application;
extern QString       issues_filter;

enum mode {
	MODE_FULL,
	MODE_RECTOR,
};

struct settings {
	QString settingsFilePath;
	QString apiKey;
	QString issuesFilter;
	enum mode mode;
	bool hideOnStart;
};
extern struct settings settings;
extern void loadSettings();
extern void saveSettings();

bool issueCmpFunct_statusIsClosed_lt ( const QJsonObject &issue_a, const QJsonObject &issue_b );
bool issueCmpFunct_statusPosition_lt ( const QJsonObject &issue_a, const QJsonObject &issue_b );
bool issueCmpFunct_statusPosition_gt ( const QJsonObject &issue_a, const QJsonObject &issue_b );
bool issueCmpFunct_updatedOn_lt ( const QJsonObject &issue_a, const QJsonObject &issue_b );
bool issueCmpFunct_updatedOn_gt ( const QJsonObject &issue_a, const QJsonObject &issue_b );
bool issueCmpFunct_name_lt ( const QJsonObject &issue_a, const QJsonObject &issue_b );
bool issueCmpFunct_name_gt ( const QJsonObject &issue_a, const QJsonObject &issue_b );
bool issueCmpFunct_assignee_lt ( const QJsonObject &issue_a, const QJsonObject &issue_b );
bool issueCmpFunct_assignee_gt ( const QJsonObject &issue_a, const QJsonObject &issue_b );
bool issueCmpFunct_dueTo_lt ( const QJsonObject &issue_a, const QJsonObject &issue_b );
bool issueCmpFunct_dueTo_gt ( const QJsonObject &issue_a, const QJsonObject &issue_b );
bool timeEntryCmpFunct_from_lt ( const QJsonObject &timeEntry_a, const QJsonObject &timeEntry_b );

#endif // COMMON_H
