#ifndef SIGNINGWINDOW_H
#define SIGNINGWINDOW_H

#include <QDialog>

namespace Ui
{
class SigningWindow;
}

class SigningWindow : public QDialog
{
	Q_OBJECT

public:
	explicit SigningWindow ( QWidget *parent = 0 );
	~SigningWindow();

signals:
	void on_destructor();

private:
	Ui::SigningWindow *ui;
};

#endif // SIGNINGWINDOW_H
