#include "planwindow.h"
#include "ui_planwindow.h"

PlanWindow::PlanWindow(MainWindowFull *mainWindow, QWidget *parent) :
	QWidget(parent),
	mainWindow(mainWindow),
	ui(new Ui::PlanWindow)
{
	this->selectedUserId = 0;

	this->setWindowTitle ( "tasks: TODO list" );
	connect ( redmine, SIGNAL ( callback_call       ( void*, callback_t, QNetworkReply*, QJsonDocument*, void* ) ),
			  this,    SLOT   ( callback_dispatcher ( void*, callback_t, QNetworkReply*, QJsonDocument*, void* ) ) );

	connect ( this->mainWindow, SIGNAL ( on_updatedIssues ), this, SLOT ( consider_updatedIssues ) );

	this->ui->setupUi(this);

	//this->ui->plan->horizontalHeader()->setSectionResizeMode ( 2, QHeaderView::Stretch );

	this->updateStuffToDo();
}

PlanWindow::~PlanWindow()
{
	this->on_destructor();
	delete this->ui;
}

void PlanWindow::consider_updatedIssues()
{

}


void PlanWindow::get_stuff_to_do_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg )
{
	( void ) reply;
	( void ) arg;
	this->updateStuffToDoMutex.lock(); // is not a thread-safe function, locking
	QJsonObject answer = json->object();
	QJsonArray doingNow    = answer["doing_now"].toArray();
	QJsonArray recommended = answer["recommended"].toArray();

	foreach ( const QJsonValue & issue, doingNow ) {
		//this->ui->plan->addRow ( issue.toObject()["id"] );
	}
	foreach ( const QJsonValue & issue, recommended ) {
		//this->ui->plan->addRow ( issue.toObject()["id"] );
	}

	//QList<QJsonObject> issues = this->mainWindow->getIssues()->get();

	this->updateStuffToDoMutex.unlock();
	this->on_updatedStuffToDo();
	return;
}

void PlanWindow::updateStuffToDo()
{
	/*int selectedUserId = this->selectedUserId;
	if (selectedUserId == 0) {
		selectedUserId = this->mainWindow->me["id"]->toInt();
	}
	this->mainWindow->getRedmine()->get_stuff_to_do ( ( Redmine::callback_t ) &PlanWindow::get_stuff_to_do_callback, this, selectedUserId, NULL, false, "");*/
	return;
}


void PlanWindow::on_user_currentIndexChanged(int index)
{
	(void) index;

	this->selectedUserId = this->ui->user->currentData().toInt();
}
