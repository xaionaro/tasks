#ifndef REDMINE_H
#define REDMINE_H

#include <RedmineClient.hpp>

class Redmine : public RedmineClient
{
private:
    QString _apiKey;

public:
    QString apiKey(QString apiKey);
    QString apiKey();

    int init();
    int request(RedmineClient::EMode mode, QString uri, const QByteArray& requestData = "");

    Redmine();
};

#endif // REDMINE_H
