#ifndef SYNTAXWINDOW_H
#define SYNTAXWINDOW_H

#include <QDialog>

namespace Ui {
class SyntaxWindow;
}

class SyntaxWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SyntaxWindow(QWidget *parent = 0);
    ~SyntaxWindow();

private slots:
    void on_closeButton_clicked();

private:
    Ui::SyntaxWindow *ui;
};

#endif // SYNTAXWINDOW_H
