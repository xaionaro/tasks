#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QTimer>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum EIcon {
        GOOD = 0,
        BAD
    };
    typedef EIcon EStatus;

    EStatus status        ()                  { return this->_status; };
    EStatus status        (EStatus newstatus) { return this->_status = newstatus; };
    EStatus statusWorsenTo(EStatus newstatus) { return this->_status = qMax(this->_status, newstatus); };

    void issue_set(int pos, QJsonObject issue);
    void issues_clear();
    void setIcon(EIcon index);
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();
    void on_actionHelp_triggered();
    void issues_doubleClick(int row, int column);
    void toggleShowHide();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    int updateTasks();

    void on_issues_itemSelectionChanged();

private:
    Ui::MainWindow *ui;

    QComboBox iconComboBox;

    QAction *showHideAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    void createIconComboBox();
    void createTrayActions();
    void createTrayIcon();

    void resizeEvent(QResizeEvent *event);
    void issuesSetup();
    QHash<int, QJsonObject> issue_row2issue;
    QTimer *timerUpdateTasks;

    EStatus _status;
};

#endif // MAINWINDOW_H
