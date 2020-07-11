#include "mainwindow-android.h"
#include "ui_mainwindow-android.h"

#include "logtimewindow.h"
#include "showtimewindow.h"

MainWindowAndroid::MainWindowAndroid ( QWidget *parent ) :
	QFrame ( parent ),
	ui ( new Ui::MainWindowAndroid )
{
	this->ui->setupUi ( this );
	this->setWindowTitle ( "tasks" );
#ifdef __WINRT__
	delete this->ui->quitButton;
#endif
}

MainWindowAndroid::~MainWindowAndroid()
{
	delete ui;
}

void MainWindowAndroid::on_quitButton_clicked()
{
	qApp->quit();
}

void MainWindowAndroid::on_logTimeWindowButton_clicked()
{
	LogTimeWindow *w = new LogTimeWindow();
	w->show();
}

void MainWindowAndroid::on_showTimeWindowButton_clicked()
{
	ShowTimeWindow *w = new ShowTimeWindow();
	w->show();
}
