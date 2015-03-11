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
#include <RedmineClient.hpp>

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

int Redmine::init() {
    this->setAuth(this->_apiKey);
    return 0;
}

int Redmine::request(
        RedmineClient::EMode    mode,
        QString                 uri,
        void *callback,
        void *callback_arg,
        const QByteArray&       requestData
) {
    this->sendRequest(uri, RedmineClient::JSON, mode,
                      callback, callback_arg, requestData);

    return 0;
}

/********* get_issues *********/

int Redmine::get_issues(void *callback, void *arg) {
    return this->request(GET, "issues", callback, arg, "");
}

int Redmine::get_issues(
        funct_callback_json callback,
        void *arg)
{
    return this->get_issues((void *)callback, arg);
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
    delete arg;
    return;
}

int Redmine::get_user(int user_id, void *callback, void *arg)
{
    return this->request(GET, "users/"+QString::number(user_id), callback, arg, "");
}

int Redmine::get_user(
        int user_id,
        funct_callback_json callback,
        void *arg)
{
    struct get_user_callback_arg *get_user_callback_arg_p = new struct get_user_callback_arg;
    //qDebug("user request: %i", user_id);

    if (this->users.contains(user_id)) {
        callback(arg, NULL, &this->users[user_id]);
        return 0;
    }

    get_user_callback_arg_p->redmine       = this;
    get_user_callback_arg_p->user_id       = user_id;
    get_user_callback_arg_p->real_callback = callback;
    get_user_callback_arg_p->arg           = arg;

    // Fix a memleak of get_user_callback_arg if this->get_user() didn't success
    return this->get_user(user_id, (void *)get_user_callback_wrapper, get_user_callback_arg_p);
}

/********* /get_user *********/
