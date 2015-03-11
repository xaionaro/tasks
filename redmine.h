#ifndef REDMINE_H
#define REDMINE_H

#include <RedmineClient.hpp>
#include <QEventLoop>

class Redmine : public RedmineClient
{
    Q_OBJECT

private:
    QString _apiKey;
    QHash <int, QJsonDocument> users;
    QHash <int, QJsonObject>   issue_statuses;

    void set_issue_status(int status_id, QJsonObject status);
    void clear_issue_status();

public slots:
    QNetworkReply *updateIssueStatuses(funct_callback_json callback = NULL, void *arg = NULL);

public:
    QString apiKey(QString apiKey);
    QString apiKey();

    int init();

    /* Request anything by URI
     */
    QNetworkReply *request(
            RedmineClient::EMode    mode,
            QString                 uri,
            void                   *callback,
            void                   *callback_arg = NULL,
            QString                 getParams = "",
            const QByteArray       &requestData = "");
    QNetworkReply *request(
            RedmineClient::EMode    mode,
            QString                 uri,
            funct_callback_json     callback,
            void                   *callback_arg = NULL,
            QString                 getParams = "",
            const QByteArray       &requestData = "");

    /* Request all issues
     */
    QNetworkReply *get_issues(void  *callback, void *arg);
    QNetworkReply *get_issues(funct_callback_json callback, void *arg);

    /* Get issue status info
     */
    QJsonObject get_issue_status(int issue_status_id);

    /* Request user info (with caching). If 2nd argument to callback function
     * is NULL then a cached value is passed.
     */
    QNetworkReply *get_user(int user_id,
            funct_callback_json callback,
            void *arg);
    QNetworkReply *get_user(int user_id, void *callback, void *arg);

    /* Next functions and variables shouldn't be called directly. It's in
     * public only due to internal reasons.
     */
    void get_user_callback(
            int user_id,
            QNetworkReply *reply,
            QJsonDocument *user,
            funct_callback_json callback,
            void *arg);
    void updateIssueStatuses_callback(
            QNetworkReply *reply,
            QJsonDocument *statuses);
    QEventLoop initBarrier;

    Redmine();
};

#endif // REDMINE_H
