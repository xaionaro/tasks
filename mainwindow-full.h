#ifndef MAINWINDOWFULL_H
#define MAINWINDOWFULL_H

#include <QTreeWidget>
#include <QTimer>

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
    bool            showProjectIssues_recursive = false;

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

private:
    Ui::MainWindowFull *ui;

    QMutex projects_display_mutex;
    void projects_display();
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
};

#endif // MAINWINDOWFULL_H
