#ifndef LOGTIMEWINDOW_H
#define LOGTIMEWINDOW_H

#include <QScrollArea>
#include <QComboBox>

#include "common.h"
#include "redmineitemtree.h"

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

    QList<QJsonObject> issues_get_byProjectId(int project_id);

    /*
     *  TODO: the next public stuff (but not slots) should be moved to
     *  the protected section:
     */
    RedmineItemTree projects;
    RedmineItemTree issues;

private slots:
    void on_cancel_clicked();

    void on_accept_clicked();

private:
    int updateLastLogTime();
    void get_time_entries_callback(QNetworkReply *reply, QJsonDocument *json, void *arg);
    QMutex updateProjectsMutex;
    QMutex updateIssuesMutex;

    QHash<int, QList<QJsonObject>> issues_byProjectId;

    int updateIssues();
    int updateProjects();

    void get_issues_callback(QNetworkReply *reply, QJsonDocument *json, void *arg);
    void issues_display();
    void setIssuesFilterItems(QComboBox *box, QHash<int,QJsonObject> table_old, QHash<int,QJsonObject> table, QString keyname);

    void get_projects_callback(QNetworkReply *reply, QJsonDocument *json, void *arg);
    void projects_display();

    Ui::LogTimeWindow *ui;
};

#endif // LOGTIMEWINDOW_H
