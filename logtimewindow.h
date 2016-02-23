#ifndef LOGTIMEWINDOW_H
#define LOGTIMEWINDOW_H

#include <QScrollArea>

namespace Ui {
class LogTimeWindow;
}

class LogTimeWindow : public QScrollArea
{
    Q_OBJECT

public:
    explicit LogTimeWindow(QWidget *parent = 0);
    ~LogTimeWindow();

private slots:
    void on_cancel_clicked();

    void on_accept_clicked();

private:
    Ui::LogTimeWindow *ui;
};

#endif // LOGTIMEWINDOW_H
