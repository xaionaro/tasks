#ifndef REDMINE_H
#define REDMINE_H

#include <RedmineClient.hpp>

class Redmine : public RedmineClient
{
    Q_OBJECT

private:
    QString _apiKey;
    QHash <int, QJsonDocument> users;

public:
    QString apiKey(QString apiKey);
    QString apiKey();

    int init();

    /* Request anything by URI
     */
    int request(
            RedmineClient::EMode mode,
            QString uri,
            void *callback,
            void *callback_arg,
            QString getParams = "",
            const QByteArray& requestData = "");

    /* Request all issues
     */
    int get_issues(void  *callback, void *arg);
    int get_issues(funct_callback_json callback, void *arg);

    /* Request user info (with caching). If 2nd argument to callback function
     * is NULL then a cached value is passed.
     */
    int get_user(int user_id,
            funct_callback_json callback,
            void *arg);
    int get_user(int user_id, void *callback, void *arg);

    /* This function shouldn't be called directly. It's in public only due to
     * internal reasons.
     */
    void get_user_callback(
            int user_id,
            QNetworkReply *reply,
            QJsonDocument *user,
            funct_callback_json callback,
            void *arg);

    Redmine();
};

#endif // REDMINE_H
