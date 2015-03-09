#ifndef REDMINE_H
#define REDMINE_H

#include <RedmineClient.hpp>

class Redmine : public RedmineClient
{
    Q_OBJECT

private:
    QString _apiKey;

public:
    QString apiKey(QString apiKey);
    QString apiKey();

    int init();

    int request(
            RedmineClient::EMode mode,
            QString uri,
            void *callback,
            void *callback_arg,
            const QByteArray& requestData = "");

    int get_issues(void  *callback, void *arg);
    int get_issues(void (*callback)(void*, QNetworkReply*, QJsonDocument*), void *arg);

    Redmine();
};

#endif // REDMINE_H
