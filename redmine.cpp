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

static void redmine_init_unlock(void *_null, QNetworkReply *reply, QJsonDocument *statuses) {
    (void)_null; (void)reply; (void)statuses;
    redmine->initBarrier.exit();
}
int Redmine::init() {
    this->setAuth(this->_apiKey);
    QNetworkReply *updateIssueStatusesReply = this->updateIssueStatuses(redmine_init_unlock);

    // Wait until issue statuses will be received:
    connect(updateIssueStatusesReply, SIGNAL(finished()), &this->initBarrier, SLOT(quit()));
    this->initBarrier.exec();
    return 0;
}

QNetworkReply *Redmine::request(
        RedmineClient::EMode    mode,
        QString                 uri,
        funct_callback_json     callback,
        void                   *callback_arg,
        bool                    free_arg,
        QString                 getParams,
        const QByteArray       &requestData
) {
    return this->request(mode, uri, (void *)callback, callback_arg, free_arg, getParams, requestData);
}

QNetworkReply *Redmine::request(
        RedmineClient::EMode    mode,
        QString                 uri,
        void                   *callback,
        void                   *callback_arg,
        bool                    free_arg,
        QString                 getParams,
        const QByteArray       &requestData
) {
    return this->sendRequest(uri, RedmineClient::JSON, mode,
                      callback, callback_arg, free_arg, getParams, requestData);
}

/********* updateIssueStatuses *********/

struct redmine_updateIssueStatuses_callback_wrapper_arg {
    funct_callback_json  real_callback;
    void                *arg;
};

void Redmine::set_issue_status(int status_id, QJsonObject status)
{
    this->issue_statuses.insert(status_id, status);
    //qDebug("status #%i: %s", status_id, status["name"].toString().toUtf8().data());
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

void Redmine::updateIssueStatuses_callback(QNetworkReply *reply, QJsonDocument *statuses_doc)
{
    (void)reply;
    int statuses_count = 0;
    QJsonArray statuses = statuses_doc->object()["issue_statuses"].toArray();

    this->clear_issue_status();

    foreach (const QJsonValue &status_val, statuses) {
        QJsonObject status = status_val.toObject();
        status["position"] = statuses_count++;
        this->set_issue_status(status["id"].toInt(), status);
    }
    return;
}

static void redmine_updateIssueStatuses_callback_wrapper(void *_arg, QNetworkReply *reply, QJsonDocument *statuses) {
    //Redmine *redmine = static_cast<Redmine *>(_redmine);
    struct redmine_updateIssueStatuses_callback_wrapper_arg *arg =
            (struct redmine_updateIssueStatuses_callback_wrapper_arg *)_arg;
    funct_callback_json  callback     = arg->real_callback;
    void                *callback_arg = arg->arg;

    redmine->updateIssueStatuses_callback(reply, statuses);
    callback(callback_arg, reply, statuses);
    return;
}

QNetworkReply *Redmine::updateIssueStatuses(funct_callback_json callback, void *arg)
{
    struct redmine_updateIssueStatuses_callback_wrapper_arg *wrapper_arg =
            (struct redmine_updateIssueStatuses_callback_wrapper_arg *)
                calloc(1, sizeof(struct redmine_updateIssueStatuses_callback_wrapper_arg));

    wrapper_arg->real_callback = callback;
    wrapper_arg->arg           = arg;

    return this->request(GET,
                         "issue_statuses",
                         redmine_updateIssueStatuses_callback_wrapper,
                         wrapper_arg,
                         true);
}

/********* /updateIssueStatuses *********/

/********* get_issues *********/

QNetworkReply *Redmine::get_issues(void *callback, void *arg, bool free_arg) {
    return this->request(GET, "issues", callback, arg, free_arg, settings.issuesFilter);
}

QNetworkReply *Redmine::get_issues(funct_callback_json callback,
        void *arg, bool free_arg)
{
    return this->get_issues((void *)callback, arg, free_arg);
}

/********* /get_issues *********/

/********* get_user *********/

struct get_user_callback_arg {
    void                *redmine;
    int                  user_id;
    funct_callback_json  real_callback;
    void                *arg;
};

// Caching function

void Redmine::get_user_callback(
        int user_id,
        QNetworkReply *reply,
        QJsonDocument *user,
        funct_callback_json callback,
        void *arg)
{
    if (user != NULL)
        this->users[user_id] = *user;

    callback(arg, reply, user);
    return;
}


static void get_user_callback_wrapper(void *_arg, QNetworkReply *reply, QJsonDocument *user) {
    struct get_user_callback_arg *arg = (struct get_user_callback_arg *)_arg;

    Redmine *redmine = static_cast<Redmine *>(arg->redmine);
    funct_callback_json callback = arg->real_callback;
    redmine->get_user_callback(arg->user_id, reply, user, callback, arg->arg);
    return;
}

QNetworkReply *Redmine::get_user(int user_id, void *callback, void *arg)
{
    return this->get_user(user_id, (funct_callback_json)callback, arg);
}

QNetworkReply *Redmine::get_user(
        int user_id,
        funct_callback_json callback,
        void *arg)
{
    struct get_user_callback_arg *get_user_callback_arg_p =
            (struct get_user_callback_arg *)calloc(1, sizeof(struct get_user_callback_arg));
    //qDebug("user request: %i", user_id);

    if (this->users.contains(user_id)) {
        callback(arg, NULL, &this->users[user_id]);
        return 0;
    }

    get_user_callback_arg_p->redmine       = this;
    get_user_callback_arg_p->user_id       = user_id;
    get_user_callback_arg_p->real_callback = callback;
    get_user_callback_arg_p->arg           = arg;

    return this->request(GET, "users/"+QString::number(user_id), get_user_callback_wrapper, get_user_callback_arg_p, true);
}

/********* /get_user *********/
