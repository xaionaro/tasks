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
};
extern struct settings settings;
extern void loadSettings();
extern void saveSettings();

#endif // COMMON_H
