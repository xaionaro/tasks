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

#include <QResizeEvent>
#include <QtAlgorithms>

#include "mainwindow-full.h"
#include "ui_mainwindow-full.h"

#include "helpwindow.h"
#include "htmldelegate.h"
#include "projectmemberswindow.h"

MainWindowFull::MainWindowFull ( QWidget *parent ) :
	MainWindowCommon ( parent ),
	ui ( new Ui::MainWindowFull )
{
	this->planWindow = NULL;

	this->ui->setupUi ( this );
	connect ( redmine, SIGNAL ( callback_call      ( void*, callback_t, QNetworkReply*, QJsonDocument*, void* ) ),
	          this,    SLOT (  callback_dispatcher ( void*, callback_t, QNetworkReply*, QJsonDocument*, void* ) ) );
	this->setWindowTitle ( "Система «Задачи» НИЯУ МИФИ" );
	this->setCorner ( Qt::TopLeftCorner,    Qt::LeftDockWidgetArea );
	this->setCorner ( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
	this->setDockOptions ( AllowTabbedDocks | AllowNestedDocks );
	this->ui->navigationDock->installEventFilter ( this );
	this->ui->issueDock->installEventFilter ( this );
	this->ui->filtersDock->installEventFilter ( this );
	this->ui->centralWidget->installEventFilter ( this );
	this->ui->navigationDock->setMinimumWidth ( this->navigationDockInitialWidth );
	this->ui->filtersDock->setMinimumWidth ( this->filtersDockInitialWidth );
	this->ui->issueDock->setMinimumHeight ( this->issueDockInitialHeight );
	QStringList projectsColumns;
	projectsColumns << "Проект";
	this->ui->projects->setHeaderLabels ( projectsColumns );
	HTMLDelegate* delegate = new HTMLDelegate();
	ui->projects->setItemDelegate ( delegate );
	QStringList issuesColumns;
	issuesColumns << "Название" << "Исполнитель" << "Срок" << "Статус" << "Обновлено";
	this->ui->issuesTree->setHeaderLabels ( issuesColumns );
	this->updateRoles();
	this->updateIssues();
	this->updateProjects();
	this->updateMemberships();
	this->updateEnumerations();
	this->timerUpdateIssues = new QTimer ( this );
	connect ( this->timerUpdateIssues, SIGNAL ( timeout() ),   this, SLOT ( updateIssues() ) );
	this->timerUpdateIssues->start ( 60000 );
	this->timerUpdateProjects = new QTimer ( this );
	connect ( this->timerUpdateProjects, SIGNAL ( timeout() ), this, SLOT ( updateProjects() ) );
	this->timerUpdateProjects->start ( 60000 );
	QDate date;
	int year_start = 2013;
	int year_cur   = qMax ( 2015, date.year() );
	int year       = year_cur;

	while ( year >= year_start )
		this->ui->issuesFilter_year->addItem ( QString::number ( year-- ) );

	this->ui->projects->setSortingEnabled ( true );
	this->ui->projects->sortByColumn ( 0, Qt::AscendingOrder );
	this->ui->projects->setSelectionMode ( QAbstractItemView::ExtendedSelection );
	this->ui->projects->setContextMenuPolicy ( Qt::CustomContextMenu );
	this->ui->issuesTree->header()->setSectionResizeMode ( 0, QHeaderView::Stretch );
	this->ui->issuesTree->header()->setStretchLastSection ( false );
	this->ui->issuesTree->setSortingEnabled ( true );
	this->ui->issuesTree->sortByColumn ( 4, Qt::DescendingOrder );
	this->ui->issuesTree->setSelectionMode ( QAbstractItemView::ExtendedSelection );
	this->ui->issuesTree->setContextMenuPolicy ( Qt::CustomContextMenu );
	/*
	connect(this->ui->projects, SIGNAL(customContextMenuRequested(const QPoint&)),
	    this, SLOT(projectsShowContextMenu(const QPoint&)));
	connect(this->ui->issuesTree, SIGNAL(customContextMenuRequested(const QPoint&)),
	    this, SLOT(issuesShowContextMenu(const QPoint&)));
	 */
	this->projectsDisplayRetryTimer.setSingleShot ( true );
	connect ( &this->projectsDisplayRetryTimer, SIGNAL ( timeout() ), this, SLOT ( projects_display() ) );
	this->ui->issuesFilter_field_assigned_to->addItem ( "", 0 );
	this->ui->issuesFilter_field_status     ->addItem ( "", 0 );

	this->createTrayActions();
	connect ( this->trayIcon, SIGNAL ( activated     ( QSystemTrayIcon::ActivationReason ) ),
			  this,			  SLOT   ( iconActivated ( QSystemTrayIcon::ActivationReason ) ) );

	return;
}

MainWindowFull::~MainWindowFull()
{
	delete ui;
}

/**** ui ****/

void unlockDockWidth ( QDockWidget *widget, QResizeEvent *event, int initialWidth )
{
	if ( event->oldSize().width() != -1 && event->size().width() != event->oldSize().width() )
		if ( event->size().width() - event->oldSize().width() > -initialWidth / 2 )
			widget->setMinimumWidth ( 0 );
}

void unlockDockHeight ( QDockWidget *widget, QResizeEvent *event, int initialHeight )
{
	if ( event->oldSize().height() != -1 && event->size().height() != event->oldSize().height() )
		if ( event->size().height() - event->oldSize().height() > -initialHeight / 2 )
			widget->setMinimumHeight ( 0 );
}

void MainWindowFull::on_resize_centralWidget ( QResizeEvent *event )
{
	/*
	    qDebug("centralWidget Resized (New Size) - Width: %d Height: %d (was: %d x %d)",
	        event->size().width(),
	        event->size().height(),
	        event->oldSize().width(),
	        event->oldSize().height()
	     );*/
	this->ui->issuesLayout->setGeometry ( QRect ( 0, 0, event->size().width(), event->size().height() - 10 ) );
	QHeaderView *issuesHeader = this->ui->issuesTree->header();
	this->ui->issuesFilter_field_name       ->resize ( issuesHeader->sectionSize ( 0 ), 24 );
	this->ui->issuesFilter_field_assigned_to->resize ( issuesHeader->sectionSize ( 1 ), 24 );
	this->ui->issuesFilter_field_due_date   ->resize ( issuesHeader->sectionSize ( 2 ), 24 );
	this->ui->issuesFilter_field_status     ->resize ( issuesHeader->sectionSize ( 3 ), 24 );
	this->ui->issuesFilter_field_updated_on ->resize ( issuesHeader->sectionSize ( 4 ), 24 );
	this->ui->issuesFilter_field_name       ->move ( issuesHeader->sectionPosition ( 0 ), 0 );
	this->ui->issuesFilter_field_assigned_to->move ( issuesHeader->sectionPosition ( 1 ), 0 );
	this->ui->issuesFilter_field_due_date   ->move ( issuesHeader->sectionPosition ( 2 ), 0 );
	this->ui->issuesFilter_field_status     ->move ( issuesHeader->sectionPosition ( 3 ), 0 );
	this->ui->issuesFilter_field_updated_on ->move ( issuesHeader->sectionPosition ( 4 ), 0 );
	return;
}

void MainWindowFull::on_resize_navigationDock ( QResizeEvent *event )
{
	unlockDockWidth ( this->ui->navigationDock, event, this->navigationDockInitialWidth );
	/*
	    qDebug("navigationDock Resized (New Size) - Width: %d Height: %d (was: %d x %d)",
	        event->size().width(),
	        event->size().height(),
	        event->oldSize().width(),
	        event->oldSize().height()
	     );*/
	this->ui->navigationTabs->resize ( event->size().width(), event->size().height() - 20 );
	this->ui->projects->resize ( event->size().width(), event->size().height() - 105 );
	return;
}

void MainWindowFull::on_resize_issueDock ( QResizeEvent *event )
{
	unlockDockHeight ( this->ui->issueDock, event, this->issueDockInitialHeight );
	qDebug ( "issueDock Resized (New Size) - Width: %d Height: %d",
	         event->size().width(),
	         event->size().height() );
	this->ui->issueLayout->setGeometry ( QRect ( 0, 0, event->size().width(), event->size().height() - 10 ) );
	this->ui->issueTitleContainer->setGeometry ( QRect ( 0, 0, event->size().width(), 30 ) );
	this->ui->issueTitle->setGeometry ( QRect ( 0, 0, event->size().width(), 30 ) );
	this->ui->issueLeftColumnWidget   ->setGeometry ( QRect ( 0,   40, 320, qMin ( event->size().height() - 70, 270 ) ) );
	this->ui->issueLeftColumn         ->setGeometry ( QRect ( 0,   0,  320, qMin ( event->size().height() - 70, 270 ) ) );
	this->ui->issueCentralColumnWidget->setGeometry ( QRect ( 320, 40, 400, qMin ( event->size().height() - 70, 270 ) ) );
	this->ui->issueCentralColumn      ->setGeometry ( QRect ( 0,   0,  400, qMin ( event->size().height() - 70, 270 ) ) );
	this->ui->issueButtonColumn->setGeometry ( QRect ( 720, 40, 130, qMin ( event->size().height() - 70, 300 ) ) );
	return;
}

void MainWindowFull::on_resize_filtersDock ( QResizeEvent *event )
{
	unlockDockWidth ( this->ui->filtersDock, event, this->filtersDockInitialWidth );
	/*
	    qDebug("filtersDock Resized (New Size) - Width: %d Height: %d",
	        event->size().width(),
	        event->size().height());*/
	this->ui->filtersLayout->setGeometry ( QRect ( 0, 0, event->size().width(), event->size().height() - 10 ) );
	return;
}

bool MainWindowFull::eventFilter ( QObject *obj, QEvent *event )
{
	if ( event->type() == QEvent::Resize ) {
		Ui::MainWindowFull *ui = this->ui;

		if ( obj == ui->navigationDock )
			this->on_resize_navigationDock ( static_cast<QResizeEvent*> ( event ) );
		else if ( obj == ui->issueDock )
			this->on_resize_issueDock     ( static_cast<QResizeEvent*> ( event ) );
		else if ( obj == ui->filtersDock )
			this->on_resize_filtersDock   ( static_cast<QResizeEvent*> ( event ) );
		else if ( obj == ui->centralWidget )
			this->on_resize_centralWidget ( static_cast<QResizeEvent*> ( event ) );
	}

	return QWidget::eventFilter ( obj, event );
}

/**** /ui ****/

/**** actions ****/

void MainWindowFull::on_actionHelp_triggered()
{
	HelpWindow *win = new HelpWindow ( this );
	win->show();
}

void MainWindowFull::on_actionQuit_triggered()
{
	application->quit();
}

void MainWindowFull::on_toolActionHelp_triggered()
{
	this->on_actionHelp_triggered();
}

void MainWindowFull::on_actionLogTime_triggered()
{
	LogTimeWindow *w = new LogTimeWindow();
	w->show();
}

void MainWindowFull::on_actionShowTime_triggered()
{
	ShowTimeWindow *w = new ShowTimeWindow();
	w->show();
}

void MainWindowFull::on_actionPlan_triggered()
{
	PlanWindow *w = new PlanWindow(this);
	w->show();
}

/**** /actions ****/

/**** updateProjects ****/

void projectWidgetItemSetText ( QWidget *__this, QTreeWidgetItem *widgetItem, QJsonObject item, RedmineItemTree *tree, int level )
{
	( void ) tree;
	( void ) level;
	int item_id = item["id"].toInt();
	MainWindowFull *_this = reinterpret_cast<MainWindowFull *> ( __this );
	widgetItem->setText ( 0,
	                      item["name"].toString() +
	                      " <span style='color: #808080'>(" +
	                      QString::number ( _this->issues_get_byProjectId ( item_id ).count() ) +
	                      ")</span>" );
	return;
}

bool projectsFilter ( QWidget *__this, QJsonObject item )
{
	MainWindowFull *_this = reinterpret_cast<MainWindowFull *> ( __this );
	( void ) _this;
	( void ) item;

	if ( item["name"].toString().indexOf ( _this->projectsFilter_namePart, Qt::CaseInsensitive ) == -1 )
		return false;

	return true;
}

void MainWindowFull::projects_display()
{
	if ( !this->projectsDisplayMutex.tryLock() ) {
		if ( !this->projectsDisplayExceptionMutex.tryLock() )
			return;

		this->projectsDisplayRetryTimer.start ( 100 );
		this->projectsDisplayExceptionMutex.unlock();
		return;
	}

	this->issues_byProjectId.clear();
	foreach ( const QJsonObject & issue, this->issues.get() ) {
		if ( this->issue_isFiltered ( issue ) )
			continue;

		QJsonObject project    = issue["project"].toObject();
		int         project_id = project["id"].toInt();
		this->issues_byProjectId[project_id].append ( issue );

		if ( this->showProjectIssues_recursive ) {
			QJsonObject parent;
			project = this->projects.get ( project["id"].toInt() );

			while ( true ) {
				int parent_id;
				parent    = this->projects.get ( project["parent"].toObject() ["id"].toInt() );
				parent_id = parent["id"].toInt();

				if ( parent_id == 0 )
					break;

				this->issues_byProjectId[parent_id].append ( issue );
				project = parent;
			}
		}
	}
	this->projects.filter ( reinterpret_cast<QWidget *> ( this ), projectsFilter );
	this->projects.display ( this->ui->projects, reinterpret_cast<QWidget *> ( this ), projectWidgetItemSetText );
	this->projectsDisplayMutex.unlock();
}

/**** /updateProjects ****/

/**** updateIssues ****/

bool MainWindowFull::issue_isFiltered ( QJsonObject issue )
{
	if ( !this->showIssues_showClosed ) {
		QJsonObject issue_status = issue["status"].toObject();
		bool        isClosed     = redmine->get_issue_status ( issue_status["id"].toInt() ) ["is_closed"].toBool();

		if ( isClosed )
			return true;
	}

	if ( this->issuesFilter_queryType != IFQT_ALL ) {
		QJsonObject author   = issue["author"].toObject();
		QJsonObject assignee = issue["assigned_to"].toObject();
		QJsonObject me       = redmine->me();
		int author_id   = author["id"].toInt();
		int assignee_id = assignee["id"].toInt();
		int me_id       = me["id"].toInt();

		switch ( this->issuesFilter_queryType ) {
			case IFQT_TOME:
				if ( ! ( assignee_id == me_id ) )
					return true;

				break;

			case IFQT_FROMME:
				if ( ! ( assignee_id != me_id && author_id == me_id ) )
					return true;

				break;

			case IFQT_FOLLOWED:
				if ( ! ( assignee_id != me_id && author_id != me_id ) )
					return true;

				break;

			case IFQT_ALL:
				qFatal ( "MainWindowFull::issue_isFiltered(): this shouldn't happend" );
		}
	}

	if ( this->issuesFilter_field_assignee_id != 0 )
		if ( issue["assigned_to"].toObject() ["id"].toInt() != this->issuesFilter_field_assignee_id )
			return true;

	if ( this->issuesFilter_field_status_id != 0 )
		if ( issue["status"].toObject() ["id"].toInt() != this->issuesFilter_field_status_id )
			return true;

	if ( this->issuesFilter_field_subjectPart.size() )
		if ( issue["subject"].toString().indexOf ( this->issuesFilter_field_subjectPart, Qt::CaseInsensitive ) == -1 )
			return true;

	return false;
}

void issuesWidgetItemSetText ( QWidget *__this, QTreeWidgetItem *widgetItem, QJsonObject issue, RedmineItemTree *tree, int level )
{
	( void ) __this;
	( void ) tree;
	( void ) level;
	//int item_id = item["id"].toInt();
	//MainWindowFull *_this = reinterpret_cast<MainWindowFull *>(__this);
	QJsonObject  issue_status = issue["status"].toObject();
//    bool         isClosed     = redmine->get_issue_status(issue_status["id"].toInt())["is_closed"].toBool();
	widgetItem->setText ( 0, issue["subject"].toString() );
	widgetItem->setText ( 1, issue["assigned_to"].toObject() ["name"].toString() );
	QString due_date_str = issue["due_date"].toString();
	QDateTime now, date;
	now  = QDateTime::currentDateTime();
	date = QDateTime::fromString ( due_date_str, "yyyy-MM-dd" );
	widgetItem->setText ( 2, due_date_str );
	widgetItem->setText ( 3, issue_status["name"].toString() );
	QDateTime updated_on = redmine->parseDateTime ( issue["updated_on"] );
	widgetItem->setText ( 4, updated_on.toString ( "yyyy'-'MM'-'dd HH':'MM" ) );
	return;
}

bool issuesFilter ( QWidget *__this, QJsonObject item )
{
	MainWindowFull *_this = reinterpret_cast<MainWindowFull *> ( __this );
	int             project_id;

	if ( _this->issue_isFiltered ( item ) )
		return false;

	if ( _this->selected_projects_id.empty() )
		return true;

	project_id = item["project"].toObject() ["id"].toInt();

	if ( _this->selected_projects_id.contains ( project_id ) )
		return true;

	if ( !_this->showProjectIssues_recursive )
		return false;

	foreach ( int selected_project_id, _this->selected_projects_id )

	if ( _this->projects.isDescendant ( project_id, selected_project_id ) )
		return true;

	return false;
}

void MainWindowFull::setIssuesFilterItems ( QComboBox *box, QHash<int, QJsonObject> table_old, QHash<int, QJsonObject> table, QString keyname )
{
	QHash<int, int> ids_old;
	foreach ( const int &item_id, table_old.keys() )
	ids_old.insert ( item_id, item_id );
	/*
	 * Adding new items
	 */
	foreach ( const int &item_id, table.keys() ) {
		if ( ids_old.contains ( item_id ) )
			ids_old.remove ( item_id );
		else
			box->addItem ( table[item_id][keyname].toString(), item_id );
	}
	/*
	 * Removing old items
	 */
	int idx = box->count();

	while ( --idx > 0 ) {
		int item_id;
		item_id = box->itemData ( idx ).toInt();

		if ( ids_old.contains ( item_id ) )
			box->removeItem ( idx );
	}

	/*
	 * Sorting
	 */
	box->model()->sort ( 0 );
	return;
}


void MainWindowFull::issues_display()
{
	qDebug ( "MainWindowFull::issues_display()" );
	this->issues.filter ( reinterpret_cast<QWidget *> ( this ), issuesFilter );
	this->issues.display ( this->ui->issuesTree, reinterpret_cast<QWidget *> ( this ), issuesWidgetItemSetText );
	QHash<int, QJsonObject> issuesFiltered_statuses_old  = this->issuesFiltered_statuses;
	QHash<int, QJsonObject> issuesFiltered_assignees_old = this->issuesFiltered_assignees;
	//QHash<int, QJsonObject> issuesFiltered_author_old    = this->issuesFiltered_author;
	this->issuesFiltered_statuses.clear();
	this->issuesFiltered_assignees.clear();
	this->issuesFiltered_authors.clear();
	foreach ( const QJsonObject & issue, this->issues.filtered.get() ) {
		QJsonObject status   = issue["status"].toObject();
		QJsonObject assignee = issue["assigned_to"].toObject();
		QJsonObject author   = issue["author"].toObject();
		int status_id   = status  ["id"].toInt();
		int assignee_id = assignee["id"].toInt();
		int author_id   = author  ["id"].toInt();
		this->issuesFiltered_authors.insert ( author_id, author );

		if ( status_id   != 0 )
			this->issuesFiltered_statuses. insert ( status_id,   status );

		if ( assignee_id != 0 )
			this->issuesFiltered_assignees.insert ( assignee_id, assignee );
	}
	this->setIssuesFilterItems ( this->ui->issuesFilter_field_assigned_to,
	                             issuesFiltered_assignees_old,
	                             this->issuesFiltered_assignees,
	                             "name" );
	this->setIssuesFilterItems ( this->ui->issuesFilter_field_status,
	                             issuesFiltered_statuses_old,
	                             this->issuesFiltered_statuses,
	                             "name" );
	this->projects_display();
	return;
}

/**** /updateIssues ****/

/**** issue_display ****/

void fill_combobox ( QComboBox *box, QHash<int, QString> values, int current_id )
{
	int idx = 0;
	foreach ( const int id, values.keys() ) {
		box->insertItem ( idx, values[id], id );

		if ( id == current_id )
			box->setCurrentIndex ( idx );

		idx++;
	}
	box->model()->sort ( 0 );
	return;
}

void fill_combobox ( QComboBox *box, QHash<int, QPair<int, QString>> values, int current_id )
{
	QList<int> position_list = values.keys();
	qSort ( position_list.begin(), position_list.end() );
	int idx = 0;
	foreach ( const int id, position_list ) {
		QPair<int, QString> value = values[id];
		box->insertItem ( idx, value.second, value.first );

		if ( value.first == current_id )
			box->setCurrentIndex ( idx );

		idx++;
	}
	return;
}

void MainWindowFull::issue_display_field ( QWidget *label, QWidget *field, int pos )
{
	QPair<QWidget *, QWidget*> row;

	if ( pos == -1 )
		pos = this->issue_field_pos++ + 100;

	row.first  = label;
	row.second = field;
	this->issue_fields[pos] = row;
	return;
}

void MainWindowFull::issue_display_dateField ( QString field_name, QString field_value )
{
	qDebug ( "issue_display_dateField: %s", field_name.toUtf8().data() );
	QLabel *label = new QLabel;
	int pos = -1;

	if ( field_name == "start_date" ) {
		label->setText ( "Начать выполнение: " );
		pos = 20;
	} else if ( field_name == "due_date" ) {
		label->setText ( "Срок выполнения: " );
		pos = 21;
	} else
		label->setText ( field_name + ": " );

	QDateEdit *field = new QDateEdit;
	QDate      date  = QDate::fromString ( field_value, "yyyy-MM-dd" );
	field->setDisplayFormat ( "yyyy-MM-dd" );
	field->setDate ( date );
	field->setCalendarPopup ( true );
	this->issue_display_field ( label, field, pos );
	return;
}

void MainWindowFull::issue_display_dateTimeField ( QString field_name, QString field_value )
{
	qDebug ( "issue_display_dateTimeField: %s", field_name.toUtf8().data() );
	QDateTime dateTime = redmine->parseDateTime ( field_value );
	QLabel *label = new QLabel;
	int pos = -1;

	if ( field_name == "created_on" ) {
		label->setText ( "Создано: " );
		pos = 10;
	} else if ( field_name == "updated_on" ) {
		label->setText ( "Обновлено: " );
		pos = 11;
	} else
		label->setText ( field_name + ": " );

	QLabel *field = new QLabel;
	field->setText ( dateTime.toString ( "yyyy-MM-dd HH:MM" ) );
	this->issue_display_field ( label, field, pos );
	return;
}

void MainWindowFull::issue_display_multilineStringField ( QString field_name, QString field_value )
{
	( void ) field_name; ( void ) field_value;
	qDebug ( "issue_display_multilineStringField: %s", field_name.toUtf8().data() );
}

void MainWindowFull::issue_display_stringField ( QString field_name, QString field_value )
{
	( void ) field_name; ( void ) field_value;
	qDebug ( "issue_display_string: %s", field_name.toUtf8().data() );
}

void MainWindowFull::issue_display_doneRatioField ( QString field_name, QString field_value )
{
	( void ) field_name; ( void ) field_value;
	qDebug ( "issue_display_doneRatioField: %s", field_name.toUtf8().data() );
}

void MainWindowFull::issue_display_enumField ( QString field_name, int field_value_id )
{
	qDebug ( "issue_display_enumField: %s", field_name.toUtf8().data() );
	QList<struct Enumerations::enumeration> enumeration = this->enumerations.get ( Enumerations::EIT_ISSUE ) [field_name];
	QLabel *label = new QLabel;

	if ( field_name == "priority" )
		label->setText ( "Приоритет: " );
	else
		label->setText ( field_name + ": " );

	QComboBox *field = new QComboBox;
	QHash<int, QPair<int, QString>> priorities;
	foreach ( const struct Enumerations::enumeration & e, enumeration ) {
		QPair <int, QString> priority;
		priority.first  = e.id;
		priority.second = e.name;
		priorities.insert ( e.position, priority );
	}
	fill_combobox ( field, priorities, field_value_id );

	if ( field_name == "priority" )
		this->issue_display_field ( label, field, 3 );
	else
		this->issue_display_field ( label, field );

	return;
}

void MainWindowFull::issue_display_intField ( QString field_name, int field_value )
{
	( void ) field_name; ( void ) field_value;
	qDebug ( "issue_display_intField: %s", field_name.toUtf8().data() );
}

void MainWindowFull::issue_display_statusField ( QString field_name, int status_id )
{
	( void ) status_id;
	qDebug ( "issue_display_statusField: %s", field_name.toUtf8().data() );
	QLabel *label = new QLabel;
	label->setText ( "Статус: " );
	QHash<int, QPair<int, QString>> statuses;
	QJsonArray allowed_statuses = this->issue["status"].toObject() ["allowed"].toArray();
	foreach ( const QJsonValue & allowed_status_val, allowed_statuses ) {
		QPair <int, QString> status;
		QJsonObject allowed_status = allowed_status_val.toObject();
		status.first  = allowed_status["id"].toInt();
		status.second = allowed_status["name"].toString();
		statuses.insert ( allowed_status["position"].toInt(), status );
	}
	QComboBox *field = new QComboBox;
	fill_combobox ( field, statuses, status_id );
	this->issue_display_field ( label, field, 2 );
	return;
}

void MainWindowFull::issue_display_trackerField ( QString field_name, int tracker_id )
{
	( void ) tracker_id;
	qDebug ( "issue_display_trackerField: %s", field_name.toUtf8().data() );
}

void MainWindowFull::issue_display_projectField ( QString field_name, int project_id )
{
	( void ) project_id;
	qDebug ( "issue_display_projectField: %s", field_name.toUtf8().data() );
}

void MainWindowFull::issue_display_categoryField ( QString field_name, int category_id )
{
	( void ) category_id;
	qDebug ( "issue_display_categoryField: %s", field_name.toUtf8().data() );
}

void MainWindowFull::issue_display_assigneeField ( QString field_name, int assignee_user_id )
{
	qDebug ( "issue_display_assigneeField: %s", field_name.toUtf8().data() );
	int project_id = this->issue["project"].toObject() ["id"].toInt();
	QHash<int, QString> assignable;
	QList<struct Memberships::membership> memberships = this->memberships.get_byproject ( project_id );
	foreach ( const struct Memberships::membership & membership, memberships )
	foreach ( const struct Memberships::membership_role & membership_role, membership.roles )

	if ( this->roles.get ( membership_role.role_id ).assignable )
		assignable.insert ( membership.user_id, membership.user_name );

	//int idx;
	QLabel *label = new QLabel;
	label->setText ( "Исполнитель: " );
	QComboBox *field = new QComboBox;
	fill_combobox ( field, assignable, assignee_user_id );
	this->issue_display_field ( label, field, 1 );
}

void MainWindowFull::issue_display_authorField ( QString field_name, int author_user_id, QString author_user_name )
{
	( void ) field_name;
	( void ) author_user_id;
	qDebug ( "issue_display_authorField: %s", field_name.toUtf8().data() );
	QLabel *label = new QLabel;
	label->setText ( "Автор: " );
	QLabel *field = new QLabel;
	field->setText ( author_user_name );
	this->issue_display_field ( label, field, 0 );
}

void MainWindowFull::issue_clear()
{
	QLayoutItem *item;

	while ( this->ui->issueLeftColumn->count() ) {
		item = this->ui->issueLeftColumn->takeAt ( 0 );

		if ( !item->widget() )
			qFatal ( "MainWindowFull::issue_clear(): Caught not implemented case" );

		delete item->widget();
		delete item;
	}

	this->issue_fields.clear();
	this->issue_field_pos = 100;
}

void MainWindowFull::issue_display_postproc()
{
	QList<int> ids = this->issue_fields.keys();
	qSort ( ids.begin(), ids.end() );
	foreach ( const int &id, ids )
		this->ui->issueLeftColumn->addRow ( this->issue_fields[id].first, this->issue_fields[id].second );
	return;
}

void MainWindowFull::issue_display ( int issue_id )
{
	QJsonObject issue;
	//QList<QString> std_simplefields << "description" << "start_date" << "due_date" << "created_on" << "updated_on";
	//QList<QString> std_arrayfields  << "project" << "tracker" << "status" << "priority" << "author" << "assigned_to";

	if ( issue_id == 0 ) {
		this->ui->issueTitle->setText ( "" );
		return;
	}

	issue = this->issues.get ( issue_id );
	this->issue_clear();
	this->issue = issue;
	this->ui->issueTitle->setText ( "[" + issue["tracker"].toObject() ["name"].toString() + " #" + QString::number ( issue["id"].toInt() ) + "] " + issue["subject"].toString() );
	QJsonObject::iterator iterator  = issue.begin();
	QJsonObject::iterator issue_end = issue.end();

	while ( iterator != issue_end ) {
		QString    key   = iterator.key();
		QJsonValue value = iterator.value();
		iterator++;

		if ( key == "custom_fields" )
			continue;

		if ( value.isString() ) { // Description, String, Date, DateTime or done_ratio
			QString value_str = value.toString();

			/*
			 * Checking if it's a Description
			 */

			if ( key == "description" ) {
				this->issue_display_multilineStringField ( key, value_str );
				continue;
			}

			/*
			 * Checking if it's a Date
			 */

			if ( key.endsWith ( "_date" ) ) {
				this->issue_display_dateField ( key, value_str );
				continue;
			}

			/*
			 * Checking if it's a DateTime
			 */

			if ( key.endsWith ( "_on" ) ) {
				this->issue_display_dateTimeField ( key, value_str );
				continue;
			}

			/*
			 * Checking if it's a "done_ratio"
			 */

			if ( key == "done_ratio" ) {
				this->issue_display_doneRatioField ( key, value_str );
				continue;
			}

			/*
			 * Otherwise it's a String
			 */
			this->issue_display_stringField ( key, value_str );
		} else if ( value.isObject() ) { // Int, Enum, Assignee, Author, Status, Tracker, Project, Category
			int     value_int = value.toObject() ["id"].toInt();
			QString value_str = value.toObject() ["name"].toString();

			/*
			 * Checking if it's an enumeration
			 */

			if ( this->enumerations.get ( Enumerations::EIT_ISSUE ).contains ( key ) ) {
				this->issue_display_enumField ( key, value_int );
				continue;
			}

			/*
			 * Checking if it's an assignee
			 */

			if ( key == "assigned_to" ) {
				this->issue_display_assigneeField ( key, value_int );
				continue;
			}

			/*
			 * Checking if it's an author
			 */

			if ( key == "author" ) {
				this->issue_display_authorField ( key, value_int, value_str );
				continue;
			}

			/*
			 * Checking if it's a status
			 */

			if ( key == "status" ) {
				this->issue_display_statusField ( key, value_int );
				continue;
			}

			/*
			 * Checking if it's a tracker
			 */

			if ( key == "tracker" ) {
				this->issue_display_trackerField ( key, value_int );
				continue;
			}

			/*
			 * Checking if it's a project
			 */

			if ( key == "project" ) {
				this->issue_display_projectField ( key, value_int );
				continue;
			}

			/*
			 * Checking if it's a project
			 */

			if ( key == "category" ) {
				this->issue_display_categoryField ( key, value_int );
				continue;
			}

			/*
			 * Otherwise it's an integer
			 */
			this->issue_display_intField ( key, value_int );
		}
	}

	this->issue_display_postproc();
	return;
}

/**** /issue_display ****/

/**** SIGNALS ****/

void MainWindowFull::on_projects_itemSelectionChanged()
{
	this->selected_projects_id.clear();
	foreach ( QTreeWidgetItem * selectedProjectItem, this->ui->projects->selectedItems() ) {
		QJsonObject project    = this->projects.get ( selectedProjectItem );
		int         project_id = project["id"].toInt();
		//qDebug("selected project: %i", project["id"].toInt());
		this->selected_projects_id.insert ( project_id, project_id );
	}
	this->issues_display();
	return;
}

void MainWindowFull::on_issuesTree_itemSelectionChanged()
{
	this->selected_issues_id.clear();
	foreach ( QTreeWidgetItem * selectedIssueItem, this->ui->issuesTree->selectedItems() ) {
		QJsonObject issue    = this->issues.get ( selectedIssueItem );
		int         issue_id = issue["id"].toInt();
		this->selected_issues_id.insert ( issue_id, issue_id );
	}

	if ( this->selected_issues_id.count() == 1 )
		this->issue_display ( this->selected_issues_id.keys().first() );
	else
		this->issue_display ( 0 );
}


void MainWindowFull::on_projectsRadio_recursive_off_toggled ( bool checked )
{
	if ( this->ui->projectsRadio_recursive_on->isChecked() == checked )
		this->ui->projectsRadio_recursive_on->setChecked ( !checked );
}

void MainWindowFull::on_projectsRadio_recursive_on_toggled ( bool checked )
{
	if ( this->ui->projectsRadio_recursive_off->isChecked() == checked )
		this->ui->projectsRadio_recursive_off->setChecked ( !checked );

	if ( this->showProjectIssues_recursive != checked ) {
		this->showProjectIssues_recursive = checked;
		this->issues_display();
	}
}

void MainWindowFull::on_issuesFilter_showClosed_yes_toggled ( bool checked )
{
	if ( this->ui->issuesFilter_showClosed_no->isChecked() == checked )
		this->ui->issuesFilter_showClosed_no->setChecked ( !checked );

	if ( this->showIssues_showClosed != checked ) {
		this->showIssues_showClosed = checked;
		this->issues_display();
	}
}

void MainWindowFull::on_issuesFilter_showClosed_no_toggled ( bool checked )
{
	if ( this->ui->issuesFilter_showClosed_yes->isChecked() == checked )
		this->ui->issuesFilter_showClosed_yes->setChecked ( !checked );
}

void MainWindowFull::on_issuesFilter_queryType_all_toggled ( bool checked )
{
	if (
	    this->ui->issuesFilter_queryType_all->isChecked() == false &&
	    this->ui->issuesFilter_queryType_toMe->isChecked() == false &&
	    this->ui->issuesFilter_queryType_fromMe->isChecked() == false &&
	    this->ui->issuesFilter_queryType_followed->isChecked() == false
	) {
		this->ui->issuesFilter_queryType_all->setChecked ( true );
	}

	if ( checked == true ) {
		this->ui->issuesFilter_queryType_followed->setChecked ( false );
		this->ui->issuesFilter_queryType_toMe->setChecked ( false );
		this->ui->issuesFilter_queryType_fromMe->setChecked ( false );
	}

	this->issuesFilter_queryType = IFQT_ALL;
	this->issues_display();
	return;
}

void MainWindowFull::on_issuesFilter_queryType_followed_toggled ( bool checked )
{
	if (
	    this->ui->issuesFilter_queryType_all->isChecked() == false &&
	    this->ui->issuesFilter_queryType_toMe->isChecked() == false &&
	    this->ui->issuesFilter_queryType_fromMe->isChecked() == false &&
	    this->ui->issuesFilter_queryType_followed->isChecked() == false
	) {
		this->ui->issuesFilter_queryType_followed->setChecked ( true );
	}

	if ( checked == true ) {
		this->ui->issuesFilter_queryType_all->setChecked ( false );
		this->ui->issuesFilter_queryType_toMe->setChecked ( false );
		this->ui->issuesFilter_queryType_fromMe->setChecked ( false );
	}

	this->issuesFilter_queryType = IFQT_FOLLOWED;
	this->issues_display();
	return;
}

void MainWindowFull::on_issuesFilter_queryType_fromMe_toggled ( bool checked )
{
	if (
	    this->ui->issuesFilter_queryType_all->isChecked() == false &&
	    this->ui->issuesFilter_queryType_toMe->isChecked() == false &&
	    this->ui->issuesFilter_queryType_fromMe->isChecked() == false &&
	    this->ui->issuesFilter_queryType_followed->isChecked() == false
	) {
		this->ui->issuesFilter_queryType_fromMe->setChecked ( true );
	}

	if ( checked == true ) {
		this->ui->issuesFilter_queryType_all->setChecked ( false );
		this->ui->issuesFilter_queryType_toMe->setChecked ( false );
		this->ui->issuesFilter_queryType_followed->setChecked ( false );
	}

	this->issuesFilter_queryType = IFQT_FROMME;
	this->issues_display();
	return;
}

void MainWindowFull::on_issuesFilter_queryType_toMe_toggled ( bool checked )
{
	if (
	    this->ui->issuesFilter_queryType_all->isChecked() == false &&
	    this->ui->issuesFilter_queryType_toMe->isChecked() == false &&
	    this->ui->issuesFilter_queryType_fromMe->isChecked() == false &&
	    this->ui->issuesFilter_queryType_followed->isChecked() == false
	) {
		this->ui->issuesFilter_queryType_toMe->setChecked ( true );
	}

	if ( checked == true ) {
		this->ui->issuesFilter_queryType_all->setChecked ( false );
		this->ui->issuesFilter_queryType_fromMe->setChecked ( false );
		this->ui->issuesFilter_queryType_followed->setChecked ( false );
	}

	this->issuesFilter_queryType = IFQT_TOME;
	this->issues_display();
	return;
}

/**** /SIGNALS ****/

void MainWindowFull::on_issuesFilter_year_currentIndexChanged ( int index )
{
	this->issuesFilter_yearIdx = index;
	this->updateIssues();
}

void MainWindowFull::on_projectFilter_field_name_textChanged ( const QString &arg1 )
{
	this->projectsFilter_namePart = arg1;
	this->issues_display();
}

void MainWindowFull::projectsShowContextMenu ( const QPoint &pos )
{
	QList<int> project_ids = this->selected_projects_id.keys();
	QPoint     globalPos   = this->ui->projects->mapToGlobal ( pos );
	QMenu contextMenu;
	QAction *membersItem = contextMenu.addAction ( "Участники" );
	QAction *selectedItem = contextMenu.exec ( globalPos );
	qDebug ( "%p %p", membersItem, selectedItem );

	if ( selectedItem == membersItem ) {
		ProjectMembersWindow *membersWindow = new ProjectMembersWindow ( this, project_ids );
		membersWindow->show();
	} else {
	}

	return;
}

void MainWindowFull::issuesShowContextMenu ( const QPoint &pos )
{
	( void ) pos;
	return;
}


void MainWindowFull::on_issuesFilter_field_name_textChanged ( const QString &arg1 )
{
	this->issuesFilter_field_subjectPart = arg1;
	this->issues_display();
}

void MainWindowFull::on_issuesFilter_field_assigned_to_currentIndexChanged ( int index )
{
	int assigned_to_id = this->ui->issuesFilter_field_assigned_to->itemData ( index ).toInt();

	if ( this->issuesFilter_field_assignee_id != assigned_to_id ) {
		this->issuesFilter_field_assignee_id  = assigned_to_id;
		this->issues_display();
	}
}

void MainWindowFull::on_issuesFilter_field_status_currentIndexChanged ( int index )
{
	int status_id = this->ui->issuesFilter_field_status->itemData ( index ).toInt();

	if ( this->issuesFilter_field_status_id != status_id ) {
		this->issuesFilter_field_status_id  = status_id;
		this->issues_display();
	}
}


/**** tray-related stuff ****/

void MainWindowFull::createTrayActions()
{
	this->showHideAction = new QAction ( tr ( "Показать/Спрятать" ), this );
	connect ( this->showHideAction, SIGNAL ( triggered() ), this, SLOT ( toggleShowHide() ) );

	this->openLogTimeWindowAction = new QAction ( tr ( "Зажурналировать время" ), this );
	connect ( this->openLogTimeWindowAction, SIGNAL ( triggered() ), this, SLOT ( openLogTimeWindow() ) );

	this->openShowTimeWindowAction = new QAction ( tr ( "Журнал времени" ), this );
	connect ( this->openShowTimeWindowAction, SIGNAL ( triggered() ), this, SLOT ( openShowTimeWindow() ) );

	this->openPlanWindowAction = new QAction ( tr ( "Оперативный план" ), this );
	connect ( this->openPlanWindowAction, SIGNAL ( triggered() ), this, SLOT ( openPlanWindow() ) );

	this->quitAction = new QAction ( tr ( "Завершить" ), this );
	connect ( this->quitAction, SIGNAL ( triggered() ), qApp, SLOT ( quit() ) );

	this->trayIconMenu->addAction ( this->showHideAction );
	this->trayIconMenu->addAction ( this->openLogTimeWindowAction );
	this->trayIconMenu->addAction ( this->openShowTimeWindowAction );
	this->trayIconMenu->addAction ( this->openPlanWindowAction );
	this->trayIconMenu->addAction ( this->quitAction );

	return;
}

void MainWindowFull::iconActivated ( QSystemTrayIcon::ActivationReason reason )
{
	qDebug("MainWindowFull::iconActivated(%i)", reason);

	switch ( reason ) {
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
			this->toggleShowHide();
			break;

		case QSystemTrayIcon::MiddleClick:
			this->openLogTimeWindow();
			break;

		default:
			break;
	}
}


/**** /tray-related stuff ****/


void MainWindowFull::on_issue_button_toOpPlan_up_clicked()
{
	/*foreach ( QTreeWidgetItem * selectedIssueItem, this->ui->issuesTree->selectedItems() ) {
		QJsonObject issue    = this->issues.get ( selectedIssueItem );
		int         issue_id = issue["id"].toInt();
	}*/
	this->on_updatedIssues();
}

void MainWindowFull::on_issue_button_removeFromOpPlan_clicked()
{
	this->on_updatedIssues();
}

void MainWindowFull::on_issue_button_toOpPlan_down_clicked()
{
	this->on_updatedIssues();
}



void MainWindowFull::on_closePlanWindow()
{
	this->planWindow = NULL;

	return;
}

void MainWindowFull::openPlanWindow()
{
	if (this->planWindow != NULL)
		delete this->planWindow;
	this->planWindow = new PlanWindow(this);

	connect ( this->planWindow, SIGNAL ( on_destructor() ), this, SLOT ( on_closePlanWindow() ) );

	this->planWindow->show();

	return;
}
