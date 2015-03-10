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

int Redmine::get_issues(void *callback, void *arg) {
    return this->request(GET, "issues", callback, arg, "");
}

int Redmine::get_issues(
        void (*callback)(void*, QNetworkReply*, QJsonDocument*),
        void *arg)
{
    return this->get_issues((void *)callback, arg);
}
