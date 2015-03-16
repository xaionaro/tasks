#ifndef MAINWINDOWFULL_H
#define MAINWINDOWFULL_H

#include <mainwindow-common.h>

namespace Ui {
class MainWindowFull;
}

class MainWindowFull : public MainWindowCommon
{
    Q_OBJECT

public:
    explicit MainWindowFull(QWidget *parent = 0);
    ~MainWindowFull();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::MainWindowFull *ui;

    void on_resize_navigationDock(QResizeEvent *event);
    void on_resize_filtersDock(QResizeEvent *event);
    void on_resize_issueDock(QResizeEvent *event);
};

#endif // MAINWINDOWFULL_H
