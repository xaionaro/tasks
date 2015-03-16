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

protected:

    static const int SORT_DEPTH = 3;

    enum ESortColumn {
        SORT_UNDEFINED = 0,

        SORT_STATUS_POS_ASC,
        SORT_STATUS_POS_DESC,

        SORT_UPDATED_ON_ASC,
        SORT_UPDATED_ON_DESC,

        SORT_STATUS_ISCLOSED_ASC,
    };

    typedef bool (*sortfunct_t)(const QJsonObject &issue_a, const QJsonObject &issue_b);

    enum ESortColumn sortColumn[SORT_DEPTH];
    QMap <enum ESortColumn, sortfunct_t> sortFunctMap;

signals:

public slots:

private:
};

#endif // MAINWINDOWCOMMON_H
