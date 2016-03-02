/*
    mephi-tasks — a client to NRNU MEPhI Redmine server

    Copyright (C) 2015  Dmitry Yu Okunev <dyokunev@ut.mephi.ru> 0x8E30679C

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow-rector.h"
#include "helpwindow.h"
#include "ui_mainwindow-rector.h"
#include "common.h"

#include <QStringList>
#include <QDesktopServices>
#include <QDateTime>
#include <QList>
#include <QMenu>
#include <QScrollBar>

void MainWindowRector::issuesSetup()
{
	QTableWidget *issues = ui->issues;
	// Columns:
	QStringList columns;
	QSize itemSize;
	columns << "Название" << "Исполнитель" << "Срок" << "Статус" << "Обновлено";
	this->sortColumnAscByIdx[0]  = SORT_NAME_ASC;
	this->sortColumnAscByIdx[1]  = SORT_ASSIGNEE_ASC;
	this->sortColumnAscByIdx[2]  = SORT_DUE_TO_ASC;
	this->sortColumnAscByIdx[3]  = SORT_STATUS_POS_ASC;
	this->sortColumnAscByIdx[4]  = SORT_UPDATED_ON_ASC;
	this->sortColumnDescByIdx[0] = SORT_NAME_DESC;
	this->sortColumnDescByIdx[1] = SORT_ASSIGNEE_DESC;
	this->sortColumnDescByIdx[2] = SORT_DUE_TO_DESC;
	this->sortColumnDescByIdx[3] = SORT_STATUS_POS_DESC;
	this->sortColumnDescByIdx[4] = SORT_UPDATED_ON_DESC;
	issues->setColumnCount ( columns.size() );
	issues->horizontalHeader()->setSectionResizeMode ( 0, QHeaderView::Stretch );
	issues->horizontalHeader()->setSectionResizeMode ( 1, QHeaderView::Interactive );
	issues->horizontalHeader()->resizeSection ( 1, 150 );
	issues->horizontalHeader()->setSectionResizeMode ( 2, QHeaderView::Interactive );
	issues->horizontalHeader()->resizeSection ( 2, 100 );
	issues->horizontalHeader()->setSectionResizeMode ( 3, QHeaderView::Interactive );
	issues->horizontalHeader()->resizeSection ( 3, 100 );
	issues->horizontalHeader()->setSectionResizeMode ( 4, QHeaderView::Interactive );
	issues->horizontalHeader()->resizeSection ( 4, 130 );
	issues->setHorizontalHeaderLabels ( columns );
	issues->horizontalHeader()->setSortIndicator ( this->sortLogicalIndex, this->sortOrder );
	//issues->verticalHeader()->setDefaultSectionSize(18);
	issues->verticalHeader()->setSectionResizeMode ( QHeaderView::ResizeToContents );
	// Signals:
	connect ( issues, SIGNAL ( cellDoubleClicked ( int, int ) ),
	          this,   SLOT  ( issues_doubleClick ( int, int ) ) );
	connect ( issues->horizontalHeader(), SIGNAL ( sectionClicked ( int ) ),
	          this,                       SLOT  ( sortColumnSwitch ( int ) ) );
	issues->horizontalHeader()->setSortIndicatorShown ( true );
	return;
}

void MainWindowRector::sortColumnSwitch ( int columnIdx )
{
	enum ESortColumn newSortColumn;
	this->sortLogicalIndex = columnIdx;
	this->sortOrder = Qt::SortOrder::AscendingOrder;
	newSortColumn = this->sortColumnAscByIdx[columnIdx];

	if ( this->sortColumn[0] == newSortColumn ) {
		this->sortOrder = Qt::SortOrder::DescendingOrder;
		newSortColumn = this->sortColumnDescByIdx[columnIdx];
	}

	this->sortColumn[0] = newSortColumn;
	this->issues_display();
}

void MainWindowRector::issues_doubleClick ( int row, int column )
{
	( void ) column;
	QString url = QString ( SERVER_URL "/issues/%1" ).arg ( this->issue_row2issue[row]["id"].toInt() );
	QDesktopServices::openUrl ( url );
	return;
}

MainWindowRector::MainWindowRector ( QWidget *parent ) :
	MainWindowCommon ( parent ),
	ui ( new Ui::MainWindowRector )
{
	ui->setupUi ( this );
	/*qDebug("MainWindowRector::MainWindowRector(): %p", this);*/
	this->setWindowTitle ( "Система «Задачи» НИЯУ МИФИ: Поручения ректора" );
	//Qt::WindowFlags flags = this->windowFlags();
	//this->setWindowFlags(flags | Qt::WindowStaysOnTopHint);
	connect ( redmine, SIGNAL ( callback_call      ( void*, callback_t, QNetworkReply*, QJsonDocument*, void* ) ),
	          this,    SLOT (  callback_dispatcher ( void*, callback_t, QNetworkReply*, QJsonDocument*, void* ) ) );
	this->issuesSetup();
	this->updateTasks();

	this->createTrayActions();
	connect ( this->trayIcon, SIGNAL ( activated     ( QSystemTrayIcon::ActivationReason ) ),
		  this,		  SLOT   ( iconActivated ( QSystemTrayIcon::ActivationReason ) ) );


	this->timerUpdateTasks = new QTimer ( this );
	connect ( this->timerUpdateTasks, SIGNAL ( timeout() ), this, SLOT ( updateTasks() ) );
	this->timerUpdateTasks->start ( 10000 );
	return;
}

struct append_assignee_arg {
	int   pos;
};

void MainWindowRector::append_assignee ( QNetworkReply *reply, QJsonDocument *coassignee_doc, void *_arg )
{
	( void ) reply;
	struct append_assignee_arg *arg = ( struct append_assignee_arg * ) _arg;
	QJsonObject coassignee = coassignee_doc->object() ["user"].toObject();
	QTableWidget *issues = this->ui->issues;
	int              pos = arg->pos;
	/*qDebug("pos: %i; answer is: %s", pos, coassignee_doc->toJson().data());*/
	QString firstname = coassignee["firstname"].toString();
	QString lastname  = coassignee["lastname"].toString();
	QString initials  = firstname.left ( 1 ) + " " + firstname.left ( firstname.indexOf ( " " ) + 2 ).right ( 1 ); // TODO: Move this to class Redmine
	QString fullname  = lastname + " " + initials;
	QTableWidgetItem *item = issues->item ( pos, 1 );
	item->setText ( item->text() + "\n  " + fullname );
	delete arg;
	return;
}

void MainWindowRector::issues_clear()
{
	/*qDebug("MainWindowRector::issues_clear(): %p", this);*/
	this->issues_list.clear();
	return;
}

void MainWindowRector::issue_display_oneissue ( int pos )
{
	QTableWidget     *issues = this->ui->issues;
	QTableWidgetItem *item;
	QJsonObject       issue        = this->issue_row2issue[pos];
	QJsonObject       issue_status = issue["status"].toObject();
	bool              isClosed = redmine->get_issue_status ( issue_status["id"].toInt() ) ["is_closed"].toBool();
	QColor            closedBgColor = QColor ( 192, 255, 192 );
	//qDebug("Issue: #%i:\t%s", issue["id"].toInt(), issue["subject"].toString().toUtf8().data());
	// New row:
	issues->insertRow ( pos );
	//     Issue name:
	item = new QTableWidgetItem ( issue["subject"].toString() );
	item->setFlags ( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

	if ( isClosed ) item->setBackgroundColor ( closedBgColor );

	issues->setItem ( pos, 0, item );
	//     Assignee:
	//         Assignee:
	QJsonObject assignee_obj = issue["assigned_to"].toObject();
	QString     assignee_str = assignee_obj["name"].toString();
	int         assignee_id  = assignee_obj["id"  ].toInt();
	//         Setting the assignee value:
	item = new QTableWidgetItem ( assignee_str );
	item->setFlags ( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

	if ( isClosed ) item->setBackgroundColor ( closedBgColor );

	issues->setItem ( pos, 1, item );
	item = issues->item ( pos, 1 );
	//         Co-assignees (asynchronous):
	QJsonArray customFields = issue["custom_fields"].toArray();
	foreach ( const QJsonValue & customField, customFields ) {
		if ( customField.toObject() ["name"].toString() == "Соисполнители" ) {
			QJsonArray coassignees_id_obj = customField.toObject() ["value"].toArray();
			foreach ( const QJsonValue & coassignee_id_obj, coassignees_id_obj ) {
				// Don't try to use .toInt() directly, the answer will always be "0":
				int coassignee_id = coassignee_id_obj.toString().toInt();

				if ( coassignee_id != assignee_id ) {
					struct append_assignee_arg *append_assignee_arg_p;
					append_assignee_arg_p = new struct append_assignee_arg;
					// TODO: fix a memleak if redmine->get_user doesn't success
					append_assignee_arg_p->pos     = pos;
					redmine->get_user ( coassignee_id,
					                    ( Redmine::callback_t ) &MainWindowRector::append_assignee,
					                    ( void * ) append_assignee_arg_p );
				}
			}
			break;
		}
	}
	//     Due date:
	QString due_date_str = issue["due_date"].toString();
	QDateTime now, date;
	now  = QDateTime::currentDateTime();
	date = QDateTime::fromString ( due_date_str, "yyyy-MM-dd" );
	item = new QTableWidgetItem ( due_date_str );
	item->setFlags ( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

	if ( isClosed ) item->setBackgroundColor ( closedBgColor );

	if ( ( due_date_str != "" ) && ( now.toTime_t() - ( 3600 * 24 - 1 ) > date.toTime_t() ) && ( !isClosed ) ) {
		item->setBackgroundColor ( QColor ( 255, 192, 192 ) );
		this->statusWorsenTo ( BAD );
	}

	issues->setItem ( pos, 2, item );
	//     Status:
	item = new QTableWidgetItem ( issue_status["name"].toString() );

	if ( isClosed ) item->setBackgroundColor ( closedBgColor );

	issues->setItem ( pos, 3, item );
	//     Updated on:
	QDateTime updated_on = redmine->parseDateTime ( issue["updated_on"] );
	item = new QTableWidgetItem ( updated_on.toString ( "yyyy'-'MM'-'dd HH':'MM" ) );

	if ( isClosed ) item->setBackgroundColor ( closedBgColor );

	issues->setItem ( pos, 4, item );
	return;
}

void MainWindowRector::issue_add ( QJsonObject issue )
{
	this->issues_list.append ( issue );
	return;
}


QList<QJsonObject> MainWindowRector::issues_get()
{
	return this->issues_list;
}

void MainWindowRector::issues_display()
{
	QTableWidget *uiIssues = this->ui->issues;
	QList<QTableWidgetSelectionRange> selected_list = uiIssues->selectedRanges();
	QList<QJsonObject>                issues_list   = this->issues_get();
	// Clearing the table
	{
		int row_count = uiIssues->rowCount();

		while ( row_count-- > 0 )
			uiIssues->removeRow ( row_count );

		this->issue_row2issue.clear();
	}
	// Filling the table
	int issues_count = 0;

	if ( this->status() == MainWindowRector::BAD )
		this->status ( MainWindowRector::GOOD );

	int scrollValue = uiIssues->verticalScrollBar()->value();
	qSort ( issues_list.begin(), issues_list.end(), this->sortFunctMap[SORT_STATUS_ISCLOSED_ASC] );

	if ( this->sortFunctMap[this->sortColumn[0]] != NULL ) {
		QList<QJsonObject>::iterator iterator,
		      issues_list_end_nonclosed = issues_list.end(),
		      issues_list_start_closed  = issues_list.end();

		for ( iterator = issues_list.begin(); iterator != issues_list.end(); iterator++ ) {
			bool isClosed = redmine->get_issue_status ( ( *iterator ) ["status"] ) ["is_closed"].toBool();

			if ( isClosed ) {
				issues_list_end_nonclosed = iterator; // TODO: find out: why here no "-1" in the expression
				issues_list_start_closed  = iterator;
				break;
			}
		}

		if ( issues_list_end_nonclosed != issues_list.begin() )
			qSort ( issues_list.begin(),      issues_list_end_nonclosed, this->sortFunctMap[this->sortColumn[0]] );

		if ( issues_list_end_nonclosed != issues_list.end() )
			qSort ( issues_list_start_closed, issues_list.end(),         this->sortFunctMap[this->sortColumn[0]] );
	}

	foreach ( const QJsonObject & issue, issues_list ) {
		this->issue_row2issue.insert ( issues_count, issue );
		this->issue_display_oneissue ( issues_count );
		issues_count++;
	}
	uiIssues->verticalScrollBar()->setValue ( scrollValue );
	foreach ( QTableWidgetSelectionRange range, selected_list )
	uiIssues->setRangeSelected (
	    QTableWidgetSelectionRange (
	        range.topRow(),    range.leftColumn(),
	        range.bottomRow(), range.rightColumn()
	    ),
	    true
	);
	this->setIcon ( this->status() );
	return;
}

void MainWindowRector::get_issues_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg )
{
	( void ) reply;
	( void ) arg;
	/*qDebug("MainWindowRector::get_issues_callback(): %p %p", this, arg);*/
	//MainWindowRector *win = static_cast<MainWindowRector *>(_win);
	QJsonObject answer = json->object();
	QJsonArray  issues = answer["issues"].toArray();
	this->issues_clear();
	foreach ( const QJsonValue & issue_val, issues )
	this->issue_add ( issue_val.toObject() );
	this->issues_display();
	return;
}

/* TODO: this function (and related ones) should be replaced with
 *     MainWindowCommon::updateIssues() [and related]
 */
int MainWindowRector::updateTasks()
{
	redmine->get_issues ( ( Redmine::callback_t ) &MainWindowRector::get_issues_callback, this );
	return 0;
}

MainWindowRector::~MainWindowRector()
{
	//saveSettings();
	delete this->ui;
	delete this->timerUpdateTasks;
}

void MainWindowRector::on_actionExit_triggered()
{
	qApp->quit();
}

void MainWindowRector::on_actionHelp_triggered()
{
	HelpWindow *win = new HelpWindow();
	win->show();
	return;
}

void MainWindowRector::toggleShowHide()
{
	if ( this->isVisible() )
		this->hide();
	else {
		this->showOnTop();
	}

	return;
}

void MainWindowRector::createTrayActions()
{
	this->showHideAction = new QAction ( tr ( "Показать/Спрятать" ), this );
	connect ( showHideAction, SIGNAL ( triggered() ), this, SLOT ( toggleShowHide() ) );
	this->quitAction = new QAction ( tr ( "Завершить" ), this );
	connect ( quitAction, SIGNAL ( triggered() ), qApp, SLOT ( quit() ) );

	this->trayIconMenu->addAction ( showHideAction );
	this->trayIconMenu->addAction ( quitAction );
}

void MainWindowRector::iconActivated ( QSystemTrayIcon::ActivationReason reason )
{
	switch ( reason ) {
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
			this->toggleShowHide();
			break;

		case QSystemTrayIcon::MiddleClick:
			break;

		default:
			break;
	}
}

void MainWindowRector::resizeEvent ( QResizeEvent *event )
{
	ui->issues->resize ( this->width(), this->height() - 50 );
	ui->labelTasksNRNUMEPhI->move ( ( this->width() - ui->labelTasksNRNUMEPhI->width() ) / 2, this->height() - 20 );
	QWidget::resizeEvent ( event );
	return;
}


#include <QThread>

void MainWindowRector::on_issues_itemSelectionChanged()
{
	QTableWidget                     *issues             = this->ui->issues;
	int                               columns_count      = issues->columnCount();
	int                               rows_count         = issues->rowCount();
	QList<QTableWidgetSelectionRange> selected_list      = issues->selectedRanges();
	foreach ( QTableWidgetSelectionRange range, selected_list ) {
		if ( range.leftColumn() != 0 || range.rightColumn() != columns_count - 1 )
			issues->setRangeSelected (
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
				issues->setRangeSelected (
				    QTableWidgetSelectionRange ( 0, 0, rows_count - 1, columns_count - 1 ),
				    false
				);
				break;
			}
	}
}
