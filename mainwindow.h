#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();
    void on_actionHelp_triggered();

private:
    int updateTasks();
    Ui::MainWindow *ui;

    QGroupBox *iconGroupBox;
    QLabel    *iconLabel;
    QComboBox *iconComboBox;
    QCheckBox *showIconCheckBox;

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    void createIconGroupBox();
    void createTrayActions();
    void createTrayIcon();

    void setIcon(int index);
};

#endif // MAINWINDOW_H
