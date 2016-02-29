#ifndef LOGTIMEWINDOW_H
#define LOGTIMEWINDOW_H

#include <QScrollArea>

#include "common.h"

namespace Ui {
class LogTimeWindow;
}

class LogTimeWindow : public QScrollArea
{
    Q_OBJECT
    CALLBACK_DISPATCHER(Redmine, LogTimeWindow, NULL)

public:
    explicit LogTimeWindow(QWidget *parent = 0);
    ~LogTimeWindow();

private slots:
    void on_cancel_clicked();

    void on_accept_clicked();

private:
    int updateLastLogTime();
    void get_time_entries_callback(QNetworkReply *reply, QJsonDocument *json, void *arg);

    Ui::LogTimeWindow *ui;
};

#endif // LOGTIMEWINDOW_H
