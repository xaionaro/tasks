#ifndef COMMON_H
#define COMMON_H

#include "redmine.h"
#define SERVER_URL "https://tasks.mephi.ru"

extern Redmine *redmine;
extern QString issues_filter;

struct settings {
    QString settingsFilePath;
    QString apiKey;
    QString issuesFilter;
};
extern struct settings settings;
extern void loadSettings();
extern void saveSettings();

#endif // COMMON_H
