/*
    mephi-tasks — a client to NRNU MEPhI Redmine server

    Copyright (C) 2015  Dmitry Yu Okunev <dyokunev@ut.mephi.ru> 0x8E30679C

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "redmine.h"
#include "common.h"

Redmine::Redmine()
{
    this->setBaseUrl(SERVER_URL);
}

QString Redmine::apiKey(QString apiKey) {
    return this->_apiKey = apiKey;
}

QString Redmine::apiKey() {
    return this->_apiKey;
}

void Redmine::init_quitStatuses(QNetworkReply *reply, QJsonDocument *statuses, void *_null) {
    (void)_null; (void)reply; (void)statuses;

    this->initBarrier_jobsDone++;

    if (this->initBarrier_jobsDone >= 2)
        this->initBarrier.exit();
}
void Redmine::init_quitMe(QNetworkReply *reply, QJsonDocument *statuses, void *_null) {
    (void)_null; (void)reply; (void)statuses;

    this->initBarrier_jobsDone++;

    if (this->initBarrier_jobsDone >= 2)
        this->initBarrier.exit();
}
int Redmine::init() {
    if (this->apiKey().length() > 0) {
        this->setAuth(this->_apiKey);
    }

    connect(this, SIGNAL(requestFinished(void*, callback_t, QNetworkReply*, QJsonDocument*, void*)),
            this, SLOT(callback_dispatcher(void*, callback_t, QNetworkReply*, QJsonDocument*, void*)));

    this->initBarrier_jobsDone = 0;

    QNetworkReply *updateIssueStatusesReply = this->updateIssueStatuses();
    // Wait until issue statuses will be received:
    connect(updateIssueStatusesReply, SIGNAL(finished()), &this->initBarrier, SLOT(quit()));
    this->initBarrier.exec();

    QNetworkReply *updateMeReply            = this->updateMe();
    // Wait until issue statuses will be received:
    connect(updateMeReply, SIGNAL(finished()), &this->initBarrier, SLOT(quit()));
    this->initBarrier.exec();

    return 0;
}

QNetworkReply *Redmine::request(RedmineClient::EMode    mode,
        QString                 uri,
        void                   *obj_ptr,
        callback_t              callback,
        void                   *callback_arg,
        bool                    free_arg,
        const QString          &getParams,
        const QByteArray       &requestData
) {
    //qDebug("request: URI: %s", uri.toUtf8().data());
    return this->sendRequest(uri, RedmineClient::JSON, mode, obj_ptr,
                      (RedmineClient::callback_t)callback, callback_arg, free_arg, getParams, requestData);
}

/********* updateMe *********/

struct redmine_updateMe_callback_arg {
    Redmine::callback_t  real_callback;
    void                *arg;
};

void Redmine::updateMe_callback(QNetworkReply *reply, QJsonDocument *me_doc, void *_arg)
{
    (void)reply;

    struct redmine_updateMe_callback_arg *arg =
            (struct redmine_updateMe_callback_arg *)_arg;
    callback_t  callback;
    void       *callback_arg;

    this->_me = me_doc->object()["user"].toObject();

    if (_arg != NULL) {
        callback     = arg->real_callback;
        callback_arg = arg->arg;
        this->callback_call(NULL, callback, reply, me_doc, callback_arg);
    }

    return;
}

QNetworkReply *Redmine::updateMe(callback_t callback, void *arg)
{
    struct redmine_updateMe_callback_arg *wrapper_arg = NULL;

    if (callback != NULL) {
         wrapper_arg =
            (struct redmine_updateMe_callback_arg *)
                calloc(1, sizeof(struct redmine_updateMe_callback_arg));

        wrapper_arg->real_callback = callback;
        wrapper_arg->arg           = arg;
    }

    return this->request(
                GET,
                "users/current",
                this,
                &Redmine::updateMe_callback,
                wrapper_arg,
                true);
}

/********* /updateMe *********/

/********* updateIssueStatuses *********/

struct redmine_updateIssueStatuses_callback_arg {
    Redmine::callback_t  real_callback;
    void                *arg;
};

void Redmine::set_issue_status(int status_id, QJsonObject status)
{
    this->issue_statuses.insert(status_id, status);
    return;
}

void Redmine::clear_issue_status()
{
    this->issue_statuses.clear();
}

QJsonObject Redmine::get_issue_status(int issue_status_id)
{
    return this->issue_statuses[issue_status_id];
}

QJsonObject Redmine::get_issue_status(QJsonValueRef issues_status_json)
{
    return this->get_issue_status(issues_status_json.toObject()["id"].toInt());
}

QHash<int, QJsonObject> Redmine::get_available_statuses_for(int issue_id) {
    (void)issue_id;
    return this->issue_statuses;

}

void Redmine::updateIssueStatuses_callback(QNetworkReply *reply, QJsonDocument *statuses_doc, void *_arg)
{
    (void)reply;

    struct redmine_updateIssueStatuses_callback_arg *arg =
            (struct redmine_updateIssueStatuses_callback_arg *)_arg;
    callback_t  callback;
    void       *callback_arg;

    int statuses_count = 0;
    QJsonArray statuses = statuses_doc->object()["issue_statuses"].toArray();

    this->clear_issue_status();

    foreach (const QJsonValue &status_val, statuses) {
        QJsonObject status = status_val.toObject();
        status["position"] = statuses_count++;
        this->set_issue_status(status["id"].toInt(), status);
    }

    if (arg != NULL) {
        callback     = arg->real_callback;
        callback_arg = arg->arg;

        this->callback_call(NULL, callback, reply, statuses_doc, callback_arg);
    }
    return;
}

QNetworkReply *Redmine::updateIssueStatuses(callback_t callback, void *arg)
{
    struct redmine_updateIssueStatuses_callback_arg *wrapper_arg = NULL;

    if (callback != NULL) {
         wrapper_arg =
            (struct redmine_updateIssueStatuses_callback_arg *)
                calloc(1, sizeof(struct redmine_updateIssueStatuses_callback_arg));

        wrapper_arg->real_callback = callback;
        wrapper_arg->arg           = arg;
    }

    return this->request(GET,
                         "issue_statuses",
                         this,
                         &Redmine::updateIssueStatuses_callback,
                         wrapper_arg,
                         true);
}

/********* /updateIssueStatuses *********/

/********* get_memberships *********/

QNetworkReply *Redmine::get_memberships(callback_t callback,
        void *arg, bool free_arg)
{
    return this->request(GET, "memberships", NULL, callback, arg, free_arg, "limit=5000");
}

/********* /get_memberships *********/

/********* get_enumerations *********/

QNetworkReply *Redmine::get_enumerations(callback_t callback,
        void *arg, bool free_arg)
{
    return this->request(GET, "enumerations", NULL, callback, arg, free_arg, "limit=5000");
}

/********* /get_enumerations *********/

/********* get_roles *********/

QNetworkReply *Redmine::get_roles(callback_t callback,
        void *arg, bool free_arg)
{
    return this->request(GET, "roles", NULL, callback, arg, free_arg, "limit=5000");
}

/********* /get_roles *********/

/********* get_issues *********/

QNetworkReply *Redmine::get_issues(callback_t callback,
        void *arg, bool free_arg)
{
    return this->request(GET, "issues", NULL, callback, arg, free_arg, settings.issuesFilter+"limit=200&status_id=*");
}

/********* /get_issues *********/

/********* get_time_entries *********/

QNetworkReply *Redmine::get_time_entries(callback_t callback,
        void *arg, bool free_arg, QString filterOptions)
{
    return this->request(GET, "time_entries", NULL, callback, arg, free_arg, filterOptions);
}

/********* /get_time_entries *********/

/********* get_projects *********/

QNetworkReply *Redmine::get_projects(callback_t callback,
        void *arg, bool free_arg)
{
    return this->request(GET, "projects", NULL, callback, arg, free_arg, "limit=500");
}

/********* /get_projects *********/

/********* get_user *********/

struct get_user_callback_arg {
    int                   user_id;
    Redmine::callback_t   real_callback;
    void                 *arg;
};

// Caching function

void Redmine::get_user_callback(
        QNetworkReply *reply,
        QJsonDocument *user_doc,
        void *_arg)
{
    struct get_user_callback_arg *arg = (struct get_user_callback_arg *)_arg;

    int        user_id  = arg->user_id;
    callback_t callback = arg->real_callback;

    if (user_doc != NULL)
        this->users[user_id] = *user_doc;

    this->callback_call(NULL, callback, reply, &this->users[user_id], arg->arg);
    return;
}

QNetworkReply *Redmine::get_user(int user_id,
        callback_t callback,
        void *arg)
{
    struct get_user_callback_arg *get_user_callback_arg_p = NULL;

    if (this->users.contains(user_id)) {
        this->callback_call(NULL, callback, NULL, &this->users[user_id], arg);
        return 0;
    }

    get_user_callback_arg_p =
            (struct get_user_callback_arg *)calloc(1, sizeof(struct get_user_callback_arg));

    get_user_callback_arg_p->user_id       = user_id;
    get_user_callback_arg_p->real_callback = callback;
    get_user_callback_arg_p->arg           = arg;

    return this->request(
                GET,
                "users/"+QString::number(user_id),
                this,
                &Redmine::get_user_callback,
                get_user_callback_arg_p,
                true);
}

/********* /get_user *********/

/********* parseDateTime *********/

QDateTime Redmine::parseDateTime(QString date_str)
{
    // TODO: FIXME: make this function working on any timezone.
    QDateTime date;

    date = QDateTime::fromString(date_str, "yyyy'-'MM'-'dd'T'HH':'mm':'ss'.'zzz'+03:00'");

    if (!date.isValid())
        // TODO: FIXME: add a hour
        date = QDateTime::fromString(date_str, "yyyy'-'MM'-'dd'T'HH':'mm':'ss'.'zzz'+04:00'");

    return date;
}

QDateTime Redmine::parseDateTime(QJsonValueRef dataTime_json) {
    return this->parseDateTime(dataTime_json.toString());
}

/********* /parseDateTime *********/
