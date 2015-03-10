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
