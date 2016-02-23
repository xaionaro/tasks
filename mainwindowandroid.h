#ifndef MAINWINDOWANDROID_H
#define MAINWINDOWANDROID_H

#include <QFrame>

namespace Ui {
class MainWindowAndroid;
}

class MainWindowAndroid : public QFrame
{
    Q_OBJECT

public:
    explicit MainWindowAndroid(QWidget *parent = 0);
    ~MainWindowAndroid();

private slots:
    void on_quitButton_clicked();

    void on_logTimeWindowButton_clicked();

private:
    Ui::MainWindowAndroid *ui;
};

#endif // MAINWINDOWANDROID_H
