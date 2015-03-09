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
    this->setAuth(this->_apiKey);
    return 0;
}

void Redmine::request_callback(QNetworkReply *reply) {
    qDebug("%p: %s", reply, (reply->isFinished() ? "finished" : "not finished"));
    qDebug(reply->errorString().toLatin1());
    QByteArray replyData = reply->readAll();
    QString replyDataLength = QString::number(replyData.length());
    info("Length: "+replyDataLength);

    QString replyText = replyData;

    info("Reply:  "+replyText);

    delete reply;

    return;
}

static void redmine_request_callback(void *_redmine, QNetworkReply *reply) {
    Redmine *redmine = static_cast<Redmine *>(_redmine);
    redmine->request_callback(reply);

    return;
}

int Redmine::request(
        RedmineClient::EMode    mode,
        QString                 uri,
        const QByteArray&       requestData
) {
    this->sendRequest(SERVER_URL+uri, mode, redmine_request_callback, this, requestData);

    return 0;
}
