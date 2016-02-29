#include "logtimewindow.h"
#include "ui_logtimewindow.h"

LogTimeWindow::LogTimeWindow(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::LogTimeWindow)
{
    this->ui->setupUi(this);
    this->ui->untilInput->setTime(QTime::currentTime());
    this->ui->sinceInput->setTime(QTime::fromString("09:00", "hh':'mm"));

    this->updateLastLogTime();
}

LogTimeWindow::~LogTimeWindow()
{
    delete ui;
}

void LogTimeWindow::get_time_entries_callback(QNetworkReply *reply, QJsonDocument *json, void *arg) {
    (void)reply; (void)arg;

    QJsonArray time_entries = json->object()["time_entries"].toArray();

    if (time_entries.count() == 0)
        return;

    QString time_entry_until;
    time_entry_until = time_entries.at(0).toObject()["until"].toString();

    QDateTime cur   = QDateTime::currentDateTime();
    QDateTime until = QDateTime::fromString(time_entry_until, "yyyy-MM-dd'T'hh:mm:ss.zzz");

    if (cur.date() > until.date())
        return;

    this->ui->sinceInput->setTime(until.time());

    return;
}

int LogTimeWindow::updateLastLogTime() {
    redmine->get_time_entries((Redmine::callback_t)&LogTimeWindow::get_time_entries_callback, this, false, "user_id=me&limit=1");
    return 0;
}

void LogTimeWindow::on_cancel_clicked()
{
    delete this;
}

void LogTimeWindow::on_accept_clicked()
{
    delete this;
}
