#ifndef MAINWINDOWFULL_H
#define MAINWINDOWFULL_H

#include <QTreeWidget>

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

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_actionHelp_triggered();

    void on_actionQuit_triggered();

    void on_toolActionHelp_triggered();

private:
    Ui::MainWindowFull *ui;

    void projects_display();

    void on_resize_centralWidget(QResizeEvent *event);
    void on_resize_navigationDock(QResizeEvent *event);
    void on_resize_filtersDock(QResizeEvent *event);
    void on_resize_issueDock(QResizeEvent *event);

    int navigationDockInitialWidth = 272;
    int filtersDockInitialWidth    = 100;
    int issueDockInitialHeight     = 300;

    QHash<int, QJsonObject> projects_row2project;
    QHash<int, QJsonObject> issues_row2issue;

    void project_display_topone(int pos);
    void project_display_child(QTreeWidgetItem *parent, QJsonObject child, int level);
    void project_display_recursive(QTreeWidgetItem *item, QJsonObject project, int level);
};

#endif // MAINWINDOWFULL_H
