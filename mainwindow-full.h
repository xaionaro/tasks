#ifndef MAINWINDOWFULL_H
#define MAINWINDOWFULL_H

#include <QTreeWidget>
#include <QTimer>
#include <QComboBox>

#include <mainwindow-common.h>

namespace Ui {
class MainWindowFull;
}

class MainWindowFull : public MainWindowCommon
{
    Q_OBJECT
    CALLBACK_DISPATCHER(Redmine, MainWindowFull, NULL)

public:
    explicit MainWindowFull(QWidget *parent = 0);
    ~MainWindowFull();

    bool issue_isFiltered(QJsonObject issue);

    /*
     *  TODO: the next public stuff should be moved to the private section:
     */
    QHash<int, int> selected_projects_id;
    QHash<int, int> selected_issues_id;
    bool            showProjectIssues_recursive = false;
    QString         projectsFilter_namePart = "";

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_actionHelp_triggered();
    void on_actionQuit_triggered();
    void on_toolActionHelp_triggered();
    void on_projects_itemSelectionChanged();
    void on_issuesTree_itemSelectionChanged();
    void on_projectsRadio_recursive_off_toggled(bool checked);
    void on_projectsRadio_recursive_on_toggled(bool checked);
    void on_issuesFilter_showClosed_yes_toggled(bool checked);
    void on_issuesFilter_showClosed_no_toggled(bool checked);
    void on_issuesFilter_queryType_all_toggled(bool checked);
    void on_issuesFilter_queryType_followed_toggled(bool checked);
    void on_issuesFilter_queryType_fromMe_toggled(bool checked);
    void on_issuesFilter_queryType_toMe_toggled(bool checked);
    void on_issuesFilter_year_currentIndexChanged(int index);
    void on_projectFilter_field_name_textChanged(const QString &arg1);
    void projects_display();

    void projectsShowContextMenu(const QPoint &pos);
    void issuesShowContextMenu(const QPoint &pos);

    void on_issuesFilter_field_name_textChanged(const QString &arg1);

    void on_issuesFilter_field_assigned_to_currentIndexChanged(int index);

    void on_issuesFilter_field_status_currentIndexChanged(int index);

private:
    Ui::MainWindowFull *ui;

    QMutex projects_display_mutex;
    QMutex issues_display_mutex;
    void issues_display();

    void on_resize_centralWidget(QResizeEvent *event);
    void on_resize_navigationDock(QResizeEvent *event);
    void on_resize_filtersDock(QResizeEvent *event);
    void on_resize_issueDock(QResizeEvent *event);

    int navigationDockInitialWidth = 272;
    int filtersDockInitialWidth    = 175;
    int issueDockInitialHeight     = 300;

    QHash<int, QJsonObject>      projects_row2project;
    QHash<int, QTreeWidgetItem*> projectItems_projectId2item;
    QHash<int, QJsonObject>      issues_row2issue;


    void project_display_topone(int pos, QHash<int, bool> &toremove_projects_id);
    void project_display_child(QTreeWidgetItem *parent, QJsonObject child, int level, QHash<int, bool> &toremove_projects_id);
    void project_display_recursive(QTreeWidgetItem *item, QJsonObject project, int level, QHash <int, bool> &toremove_projects_id);

    void projectResetIfUpdated(int project_id, QJsonObject project);
    void projectResetRecursive(int project_id);

    void issue_display(int issue_id);

    void setIssuesFilterItems(QComboBox *box, QHash<int, QJsonObject> table_old, QHash<int, QJsonObject> table, QString keyname);

    bool showIssues_showClosed = false;

    //QList<QJsonObject> selected_issues;

    QTimer *timerUpdateIssues;
    QTimer *timerUpdateProjects;

    enum issuesFilter_queryType {
        IFQT_ALL,
        IFQT_TOME,
        IFQT_FROMME,
        IFQT_FOLLOWED,
    };

    enum issuesFilter_queryType issuesFilter_queryType = IFQT_ALL;

    int issuesFilter_yearIdx = 0;

    QMutex projectsDisplayMutex;
    QMutex projectsDisplayExceptionMutex;

    QTimer projectsDisplayRetryTimer;

    QString issuesFilter_field_subjectPart = "";

    QHash <int, QJsonObject> issuesFiltered_statuses;
    QHash <int, QJsonObject> issuesFiltered_authors;
    QHash <int, QJsonObject> issuesFiltered_assignees;

    int issuesFilter_field_assignee_id = 0;
    int issuesFilter_field_status_id   = 0;

    void issue_display_field(QWidget *label, QWidget *field);

    void issue_display_dateField(QString field_name, QString field_value);
    void issue_display_stringField(QString field_name, QString field_value);
    void issue_display_enumField(QString field_name, int field_value_id);
    void issue_display_dateTimeField(QString field_name, QString field_value);
    void issue_display_assigneeField(QString field_name, int assignee_user_id);
    void issue_display_authorField(QString field_name, int assignee_user_id, QString author_user_name);
    void issue_display_multilineStringField(QString field_name, QString field_value);
    void issue_display_doneRatioField(QString field_name, QString field_value);
    void issue_display_intField(QString field_name, int field_value);
    void issue_display_statusField(QString field_name, int status_id);
    void issue_display_trackerField(QString field_name, int tracker_id);
    void issue_display_projectField(QString field_name, int project_id);


    void issue_clear();

    int issue_columns    = 3;
    int issue_column_cur = 0;

    QJsonObject issue;
};

#endif // MAINWINDOWFULL_H
