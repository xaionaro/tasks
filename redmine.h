#ifndef REDMINE_H
#define REDMINE_H

#include <RedmineClient.hpp>
#include <QEventLoop>


class Redmine : public RedmineClient
{
    Q_OBJECT
    CALLBACK_DISPATCHER(RedmineClient, Redmine, this)


private:
    QString _apiKey;
    QHash <int, QJsonDocument> users;
    QHash <int, QJsonObject>   issue_statuses;

    void set_issue_status(int status_id, QJsonObject status);
    void clear_issue_status();

    // Callbacks:
    void updateIssueStatuses_callback(QNetworkReply *reply,
            QJsonDocument *statuses,
            void *_arg);

    void init_quit(QNetworkReply *reply, QJsonDocument *statuses, void *_null);

    void get_user_callback(QNetworkReply *reply,
            QJsonDocument *user_doc,
            void *_arg);
    QEventLoop initBarrier;

public:

    QString apiKey(QString apiKey);
    QString apiKey();

    int init();

    /* Request anything by URI
     */
    QNetworkReply *request(RedmineClient::EMode    mode,
            QString                 uri,
            void                   *obj_ptr,
            callback_t              callback,
            void                   *callback_arg = NULL,
            bool                    free_arg     = false,
            const QString          &getParams    = "",
            const QByteArray       &requestData  = "");

    /* Request all issues
     */
    QNetworkReply *get_issues(callback_t callback, void *arg, bool free_arg = false);

    /* Get issue status info
     */
    QJsonObject get_issue_status(int issue_status_id);

    /* Request user info (with caching). If 2nd argument to callback function
     * is NULL then a cached value is passed.
     */
    QNetworkReply *get_user(int user_id,
            callback_t callback,
            void *arg);

    Redmine();

private slots:
    QNetworkReply *updateIssueStatuses(Redmine::callback_t callback = NULL, void *arg = NULL);

};

#endif // REDMINE_H
