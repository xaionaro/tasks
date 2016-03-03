#include "showtimewindow.h"
#include "ui_showtimewindow.h"

ShowTimeWindow::ShowTimeWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ShowTimeWindow)
{
	ui->setupUi(this);

	connect ( redmine, SIGNAL ( callback_call       ( void*, callback_t, QNetworkReply*, QJsonDocument*, void* ) ),
		  this,    SLOT   ( callback_dispatcher ( void*, callback_t, QNetworkReply*, QJsonDocument*, void* ) ) );

	this->updateUsers();
	this->updateTimeEntries();
}

ShowTimeWindow::~ShowTimeWindow()
{
	delete ui;
}

/********* updateUsers *********/

void ShowTimeWindow_usersAddItemFunct ( QComboBox *users, QJsonObject user )
{
	int userId = user["id"].toInt();
	QString userDisplayName = user["name"].toString();
	users->addItem ( userDisplayName, userId );
	return;
}


bool ShowTimeWindow_usersFilter ( QWidget *__this, QJsonObject item )
{
	ShowTimeWindow *_this = reinterpret_cast<ShowTimeWindow *> ( __this );
	( void ) _this;
	( void ) item;

	return true;
}

void ShowTimeWindow::updateUsers_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg )
{
	( void ) reply;
	( void ) arg;

	QJsonArray  users = json->object() ["users"].toArray();

	qDebug ( "ShowTimeWindow::updateUsers_callback: users.count() == %i", users.count() );

	this->ui->user->clear();
	this->ui->user->addItem ( "Я", 0 );
	this->users.set ( users );
	this->users.filter ( reinterpret_cast<QWidget *> ( this ), ShowTimeWindow_usersFilter );
	this->users.display ( this->ui->user, reinterpret_cast<QWidget *> ( this ), ShowTimeWindow_usersAddItemFunct );

	return;
}

void ShowTimeWindow::updateUsers()
{
	redmine->get_stuff_to_do ( this, ( Redmine::callback_t ) &ShowTimeWindow::updateUsers_callback );
	return;
}

/********* /updateUsers *********/

void ShowTimeWindow::updateTimeEntries()
{
	return;
}
