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
	this->updateTimeEntries(0);

	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 0, QHeaderView::Interactive );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 0, 50 );
	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 1, QHeaderView::Interactive );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 1, 50 );
	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 2, QHeaderView::Interactive );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 2, 45 );
	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 3, QHeaderView::Interactive );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 3, 70 );
	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 4, QHeaderView::Interactive );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 4, 60 );
	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 5, QHeaderView::Interactive );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 5, 200 );
	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 6, QHeaderView::Stretch );

	this->ui->timeEntries->horizontalHeader()->setSortIndicatorShown ( true );
}

ShowTimeWindow::~ShowTimeWindow()
{
	delete ui;
}

/********* updateUsers *********/


void ShowTimeWindow::updateUsers_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg )
{
	( void ) reply;
	( void ) arg;

	QJsonArray  users = json->object() ["users"].toArray();

	qDebug ( "ShowTimeWindow::updateUsers_callback: users.count() == %i", users.count() );

	this->ui->user->clear();
	this->ui->user->addItem ( "Я", 0 );

	foreach (const QJsonValue &userV, users) {
		QJsonObject user = userV.toObject();

		int userId = user["id"].toInt();
		QString userDisplayName = user["name"].toString();
		this->ui->user->addItem ( userDisplayName, userId );
	}

	return;
}

void ShowTimeWindow::updateUsers()
{
	redmine->get_stuff_to_do ( this, ( Redmine::callback_t ) &ShowTimeWindow::updateUsers_callback );
	return;
}

/********* /updateUsers *********/

/********* updateTimeEntries *********/

void ShowTimeWindow::timeEntries_display()
{
	//this->ui->timeEntries->clear();
	this->ui->timeEntries->setRowCount ( this->timeEntries.count() );

	QDate dateSelected = this->ui->date->selectedDate();

	int row = 0;
	foreach (const QJsonValue &timeEntryV, this->timeEntries) {
		QJsonObject timeEntry = timeEntryV.toObject();
		QDateTime from = QDateTime::fromString(timeEntry ["from"].toString(), Qt::ISODate);
		QDateTime to   = QDateTime::fromString(timeEntry ["to"  ].toString(), Qt::ISODate);
		int sec_diff  = from.time().secsTo(to.time());

		QDate date = from.date();

		if (date != dateSelected)
			continue;

		qDebug ( "\"from\": \"%s\", sec_diff == %i; from.minute() == %i; to.minute() == %i", timeEntry ["from"].toString().toStdString().c_str(), sec_diff, from.time().minute(), to.time().minute() );

		this->ui->timeEntries->setItem(row, 0, new QTableWidgetItem ( from.toString("hh:mm") ) );
		this->ui->timeEntries->setItem(row, 1, new QTableWidgetItem ( to  .toString("hh:mm") ) );
		this->ui->timeEntries->setItem(row, 2, new QTableWidgetItem ( QString::number( sec_diff/60 ) ) );
		this->ui->timeEntries->setItem(row, 3, new QTableWidgetItem ( timeEntry ["activity"].toObject() ["name"].toString() ) );

		if (timeEntry ["issue"].toObject().empty()) {
			QString projectIdentifier = timeEntry ["project"].toObject() ["identifier"].toString();
			QString projectName;
			if ( projectIdentifier == timeEntry ["user"].toObject() ["login"].toString() ) {
				projectName = "Личный проект";
			} else {
				projectName = timeEntry ["project"].toObject() ["name"].toString();
			}
			this->ui->timeEntries->setItem(row, 4, new QTableWidgetItem ( projectIdentifier ) );
			this->ui->timeEntries->setItem(row, 5, new QTableWidgetItem ( projectName       ) );
		} else {
			this->ui->timeEntries->setItem(row, 4, new QTableWidgetItem ( QString::number(timeEntry ["issue"].toObject()    ["id"].toInt()) ) );
			this->ui->timeEntries->setItem(row, 5, new QTableWidgetItem ( timeEntry ["issue"].toObject()    ["subject"].toString() ) );
		}

		this->ui->timeEntries->setItem(row, 6, new QTableWidgetItem ( timeEntry ["comments"].toString() ) );

		row++;
	}

	this->ui->timeEntries->setRowCount ( row );
}

void ShowTimeWindow::updateTimeEntries_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg )
{
	( void ) reply;
	( void ) arg;

	this->timeEntries = json->object() ["time_entries"].toArray();

	qDebug ( "ShowTimeWindow::updateTimeEntries_callback: timeEntries.count() == %i", timeEntries.count() );

	this->timeEntries_display();
	return;
}

void ShowTimeWindow::updateTimeEntries(int userId)
{
	redmine->get_time_entries( userId, this, ( Redmine::callback_t ) &ShowTimeWindow::updateTimeEntries_callback, NULL, false, "limit=1000" );
	return;
}

/********* /updateTimeEntries *********/

void ShowTimeWindow::on_closeButton_clicked()
{
	delete this;
}

void ShowTimeWindow::on_date_selectionChanged()
{
	this->timeEntries_display();
}
