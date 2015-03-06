#include "redmine.h"
#include "common.h"
#include <RedmineClient.hpp>


Redmine::Redmine()
{

}

QString Redmine::apiKey(QString apiKey) {
    return this->_apiKey = apiKey;
}

QString Redmine::apiKey() {
    return this->_apiKey;
}

int Redmine::init() {
    //this->setAuth(this->_apiKey);
    return 0;
}

int Redmine::request(
        RedmineClient::EMode    mode,
        QString                 uri,
        const QByteArray&       requestData
) {
    //this->sendRequest(SERVER_URL+uri, mode, requestData);
    return 0;
}
