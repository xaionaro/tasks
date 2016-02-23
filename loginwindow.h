#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include "redmine.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QDialog *parent = 0);
    ~LoginWindow();
    QString resultApiKey;

signals:

public slots:

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H
