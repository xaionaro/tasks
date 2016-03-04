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
	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 1, QHeaderView::Interactive );
	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 2, QHeaderView::Interactive );
	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 3, QHeaderView::Interactive );
	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 4, QHeaderView::Interactive );
	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 5, QHeaderView::Interactive );

#ifdef __MOBILE__
	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 6, QHeaderView::Interactive );

	this->ui->timeEntries->horizontalHeader()->resizeSection ( 0, 80 );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 1, 80 );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 2, 72 );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 3, 104 );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 4, 82 );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 5, 500 );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 6, 500 );
#else
	this->ui->timeEntries->horizontalHeader()->setSectionResizeMode ( 6, QHeaderView::Stretch );

	this->ui->timeEntries->horizontalHeader()->resizeSection ( 0, 50 );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 1, 50 );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 2, 45 );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 3, 70 );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 4, 60 );
	this->ui->timeEntries->horizontalHeader()->resizeSection ( 5, 200 );

	this->ui->date->setMaximumHeight(120);
#endif

	this->ui->timeEntries->horizontalHeader()->setSortIndicatorShown ( true );

	this->ui->date->setSelectedDate(QDate::currentDate());
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

		// To make a cache:
		redmine->get_time_entries( userId, NULL, NULL, NULL, false, "limit=1000" );
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
	QList<QJsonObject> list;
	//this->ui->timeEntries->clear();
	//this->ui->timeEntries->setRowCount ( this->timeEntries.count() );

	QDate dateSelected = this->ui->date->selectedDate();

	foreach (const QJsonValue &timeEntryV, this->timeEntries) {
		QJsonObject timeEntry = timeEntryV.toObject();
		QDateTime from = QDateTime::fromString(timeEntry ["from"].toString(), Qt::ISODate);

		QDate date = from.date();

		if (date != dateSelected)
			continue;

		list.append(timeEntry);
	}

	qSort ( list.begin(), list.end(), timeEntryCmpFunct_from_lt );

	this->ui->timeEntries->setRowCount ( list.size() );

	for (int row = 0; row < list.size(); ++row) {
	    QJsonObject timeEntry = list.at(row);

	    QDateTime from = QDateTime::fromString(timeEntry ["from"].toString(), Qt::ISODate);
	    QDateTime to   = QDateTime::fromString(timeEntry ["to"  ].toString(), Qt::ISODate);
	    int sec_diff  = from.time().secsTo(to.time());

	    //qDebug ( "\"from\": \"%s\", sec_diff == %i; from.minute() == %i; to.minute() == %i", timeEntry ["from"].toString().toStdString().c_str(), sec_diff, from.time().minute(), to.time().minute() );

	    this->ui->timeEntries->setItem(row, 0, new QTableWidgetItem ( from.toString("hh:mm") ) );
	    this->ui->timeEntries->setItem(row, 1, new QTableWidgetItem ( to  .toString("hh:mm") ) );
	    this->ui->timeEntries->setItem(row, 2, new QTableWidgetItem ( QString::number( sec_diff/60 ) ) );
	    this->ui->timeEntries->setItem(row, 3, new QTableWidgetItem ( timeEntry ["activity"].toObject() ["name"].toString() ) );

	    if (timeEntry ["issue"].toObject().empty()) {
		    QString projectIdentifier = timeEntry ["project"].toObject() ["identifier"].toString();
		    QString projectName;
		    QString userLogin = timeEntry ["user"].toObject() ["login"].toString();

		    if ( projectIdentifier.toLower() == userLogin.toLower() ) {
			    projectName = "Личный проект";
		    } else {
			    projectName = timeEntry ["project"].toObject() ["name"].toString();
		    }

		    //qDebug(("projectIdentifier == \""+projectIdentifier+"\"; userLogin == \""+userLogin+"\"; projectName == \""+projectName+"\"").toStdString().c_str());

		    this->ui->timeEntries->setItem(row, 4, new QTableWidgetItem ( projectIdentifier ) );
		    this->ui->timeEntries->setItem(row, 5, new QTableWidgetItem ( projectName       ) );
	    } else {
		    this->ui->timeEntries->setItem(row, 4, new QTableWidgetItem ( QString::number(timeEntry ["issue"].toObject()    ["id"].toInt()) ) );
		    this->ui->timeEntries->setItem(row, 5, new QTableWidgetItem ( timeEntry ["issue"].toObject()    ["subject"].toString() ) );
	    }

	    this->ui->timeEntries->setItem(row, 6, new QTableWidgetItem ( timeEntry ["comments"].toString() ) );
	}
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

void ShowTimeWindow::on_timeEntries_itemSelectionChanged()
{
	QTableWidget                     *timeEntries        = this->ui->timeEntries;
	int                               columns_count      = timeEntries->columnCount();
	int                               rows_count         = timeEntries->rowCount();
	QList<QTableWidgetSelectionRange> selected_list      = timeEntries->selectedRanges();
	foreach ( QTableWidgetSelectionRange range, selected_list ) {
		if ( range.leftColumn() != 0 || range.rightColumn() != columns_count - 1 )
			timeEntries->setRangeSelected (
			    QTableWidgetSelectionRange (
				range.topRow(),    0,
				range.bottomRow(), columns_count - 1
			    ),
			    true
			);
		else

			/* Workaround: Drop selection if everything is selected
			 * it's required to do not select everything on sort switching
			 */
			if ( range.leftColumn() == 0 && range.rightColumn() == columns_count - 1 &&
			     range.topRow()     == 0 && range.bottomRow()   == rows_count - 1 ) {
				timeEntries->setRangeSelected (
				    QTableWidgetSelectionRange ( 0, 0, rows_count - 1, columns_count - 1 ),
				    false
				);
				break;
			}
	}
}

void ShowTimeWindow::on_user_currentIndexChanged(int index)
{
	(void) index;

	int userId = this->ui->user->currentData().toInt();

	this->updateTimeEntries(userId);

	return;
}
