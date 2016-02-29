#include "logtimewindow.h"
#include "ui_logtimewindow.h"

#include "redmineclass_time_entry.h"

LogTimeWindow::LogTimeWindow(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::LogTimeWindow)
{
    this->ui->setupUi(this);
    this->ui->untilInput->setTime(QTime::currentTime());
    this->ui->sinceInput->setTime(QTime::fromString("09:00", "hh':'mm"));


    connect(redmine, SIGNAL(callback_call      (void*,callback_t,QNetworkReply*,QJsonDocument*,void*)),
            this,    SLOT(  callback_dispatcher(void*,callback_t,QNetworkReply*,QJsonDocument*,void*)) );

    this->updateLastLogTime();
}

LogTimeWindow::~LogTimeWindow()
{
    delete ui;
}

void LogTimeWindow::get_time_entries_callback(QNetworkReply *reply, QJsonDocument *json, void *arg) {
    (void)reply; (void)arg;

    // Using "to" field of the last timelog entry as a "since"/"from" value for a new entry

    QJsonArray time_entries = json->object()["time_entries"].toArray();

    qDebug("get_time_entries_callback: time_entries.count() == %i", time_entries.count());

    if (time_entries.count() == 0)
        return;

    QString time_entry_until;
    time_entry_until = time_entries.at(0).toObject()["to"].toString();

    QDateTime cur   = QDateTime::currentDateTime();
    QDateTime until = QDateTime::fromString(time_entry_until, Qt::ISODate);

    qDebug("get_time_entries_callback: time_entry_until == \"%s\"", time_entry_until.toStdString().c_str());

    if (cur.date() > until.date()) {
        qDebug("get_time_entries_callback: cur.date() > until.date(): %s > %s", cur.toString(Qt::ISODate).toStdString().c_str(), until.toString(Qt::ISODate).toStdString().c_str());
        return;
    }

    this->ui->sinceInput->setTime(until.time());

    return;
}

int LogTimeWindow::updateLastLogTime() {
    qDebug("updateLastLogTime()");
    redmine->get_time_entries((Redmine::callback_t)&LogTimeWindow::get_time_entries_callback, this, false, "user_id=me&limit=1");
    return 0;
}

void LogTimeWindow::on_cancel_clicked()
{
    delete this;
}

void LogTimeWindow::on_accept_clicked()
{
    RedmineClass_TimeEntry timeEntry;

    timeEntry.setRedmine(redmine);
    timeEntry.set(this->ui->sinceInput->dateTime(), this->ui->untilInput->dateTime(), 0, this->ui->comment->toPlainText());
    timeEntry.save();

    delete this;
}
