#include "mainwindowandroid.h"
#include "ui_mainwindowandroid.h"

#include "logtimewindow.h"

MainWindowAndroid::MainWindowAndroid ( QWidget *parent ) :
	QFrame ( parent ),
	ui ( new Ui::MainWindowAndroid )
{
	this->ui->setupUi ( this );
	this->setWindowTitle ( "Система «Задачи» НИЯУ МИФИ" );
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
