#ifndef MAINWINDOWCOMMON_H
#define MAINWINDOWCOMMON_H

#include <QMainWindow>
#include <QTableWidgetSelectionRange>

#include "common.h"

class MainWindowCommon : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowCommon(QWidget *parent = 0);
    ~MainWindowCommon();

protected:

    static const int SORT_DEPTH = 3;

    enum ESortColumn {
        SORT_UNDEFINED = 0,

        SORT_NAME_ASC,
        SORT_NAME_DESC,

        SORT_ASSIGNEE_ASC,
        SORT_ASSIGNEE_DESC,

        SORT_DUE_TO_ASC,
        SORT_DUE_TO_DESC,

        SORT_STATUS_POS_ASC,
        SORT_STATUS_POS_DESC,

        SORT_UPDATED_ON_ASC,
        SORT_UPDATED_ON_DESC,

        SORT_STATUS_ISCLOSED_ASC,
    };

    typedef bool (*sortfunct_t)(const QJsonObject &issue_a, const QJsonObject &issue_b);

    // TODO: deduplicate sortLogicalIndex+sortOrder and sortColumn
    int sortLogicalIndex = -1;
    Qt::SortOrder sortOrder;

    enum ESortColumn sortColumn[SORT_DEPTH];
    QMap <enum ESortColumn, sortfunct_t> sortFunctMap;

    int updateProjects();
    QList <QJsonObject> projects_get();

signals:

public slots:

private:
    /* projects */

    virtual void projects_display();
    void get_projects_callback(QNetworkReply *reply, QJsonDocument *json, void *arg);
    void projects_clear();
    void project_add(QJsonObject project_json);
    QList <QJsonObject> projects_list;
};

#endif // MAINWINDOWCOMMON_H
