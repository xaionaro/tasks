#ifndef COMMON_H
#define COMMON_H

#include "redmine.h"
#define SERVER_URL "https://tasks.mephi.ru"

extern Redmine *redmine;
extern QString issues_filter;

extern int info(QString messageText);

#endif // COMMON_H
