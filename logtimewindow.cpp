#include "logtimewindow.h"
#include "ui_logtimewindow.h"

#include <QDesktopServices>
#include <QMessageBox>

LogTimeWindow::LogTimeWindow ( QWidget *parent ) :
	QScrollArea ( parent ),
	ui ( new Ui::LogTimeWindow )
{
	this->ui->setupUi ( this );

	connect ( &this->timeEntry, SIGNAL ( on_saveSuccess() ), this, SLOT ( on_saveSuccess() ) );
	connect ( &this->timeEntry, SIGNAL ( on_saveTimeout() ), this, SLOT ( on_saveTimeout() ) );
	connect ( &this->timeEntry, SIGNAL ( on_saveFailure(QNetworkReply *) ), this, SLOT ( on_saveFailure(QNetworkReply *) ) );

	this->timeEntry.setRedmine ( redmine );

	QTime currentTime      = QTime::currentTime();
	QTime initialSinceTime = QTime::fromString ( "09:00", "hh':'mm" );
	if (initialSinceTime > currentTime) {
		initialSinceTime = currentTime;
	}

	this->ui->untilInput->setDate ( QDate::currentDate() );
	this->ui->sinceInput->setDate ( QDate::currentDate() );
	this->ui->untilInput->setTime ( currentTime );
	this->ui->sinceInput->setTime ( initialSinceTime );
	this->setWindowTitle ( "Система «Задачи» НИЯУ МИФИ: Учёт времени" );
	connect ( redmine, SIGNAL ( callback_call       ( void*, callback_t, QNetworkReply*, QJsonDocument*, void* ) ),
              this,    SLOT   ( callback_dispatcher ( void*, callback_t, QNetworkReply*, QJsonDocument*, void* ) ) );
	this->selected_project_id = 0;
	this->timeEntry.setProjectId ( redmine->myProject() ["id"].toInt() );
	this->updateLastLogTime();
	this->updateIssues();
	this->updateProjects();
	this->ui->issue->setSortingEnabled ( true );
	this->ui->issue->sortByColumn ( 0, Qt::AscendingOrder );
}

LogTimeWindow::~LogTimeWindow()
{
	this->on_destructor();
	delete ui;
}

void LogTimeWindow::on_saveSuccess()
{
	qDebug ( "LogTimeWindow::on_saveSuccess()" );
	redmine->get_time_entries ( ( void * ) NULL, NULL, NULL, false, "user_id=me&limit=1" ); // Just to update the cache
	//delete this;
}


void LogTimeWindow::on_saveTimeout()
{
	qDebug ( "LogTimeWindow::on_saveTimeout()" );
	QMessageBox messageBox;
	messageBox.critical(0, "Error", "Превышен интервал ожидания ответа. Проверьте качество подключения к сети «Интернет».");
}


void LogTimeWindow::on_saveFailure ( QNetworkReply *reply )
{
	( void ) reply;

	qDebug ( "LogTimeWindow::on_saveFailure()" );

	QString techInfo = QString("issueId == %1; projectId == %2").arg(this->timeEntry.getIssueId()).arg(this->timeEntry.getProjectId());
	QMessageBox messageBox;
	messageBox.critical(0, "Error", "Внутренняя ошибка. Рекомендуем связаться с технической поддержкой по адресу <tasks@mephi.ru>. Техническая информация: "+techInfo);

	return;
}

void LogTimeWindow::get_time_entries_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg )
{
	( void ) reply;
	( void ) arg;
	// Using "to" field of the last timelog entry as a "since"/"from" value for a new entry
	QJsonArray time_entries = json->object() ["time_entries"].toArray();
	qDebug ( "get_time_entries_callback: time_entries.count() == %i", time_entries.count() );

	if ( time_entries.count() == 0 )
		return;

	QString time_entry_until;
	time_entry_until = time_entries.at ( 0 ).toObject() ["to"].toString();
	QDateTime cur   = QDateTime::currentDateTime();
	QDateTime until = QDateTime::fromString ( time_entry_until, Qt::ISODate );
	qDebug ( "get_time_entries_callback: time_entry_until == \"%s\"", time_entry_until.toStdString().c_str() );

	if ( cur.date() > until.date() ) {
		qDebug ( "get_time_entries_callback: cur.date() > until.date(): %s > %s", cur.toString ( Qt::ISODate ).toStdString().c_str(), until.toString ( Qt::ISODate ).toStdString().c_str() );
		return;
	}

	this->ui->sinceInput->setTime ( until.time() );
	return;
}

int LogTimeWindow::updateLastLogTime()
{
	qDebug ( "updateLastLogTime()" );
	redmine->get_time_entries ( this, ( Redmine::callback_t ) &LogTimeWindow::get_time_entries_callback, this, false, "user_id=me&limit=1" );
	return 0;
}

void LogTimeWindow::on_cancel_clicked()
{
	delete this;
}

void LogTimeWindow::on_accept_clicked()
{
	this->saveCurrentEntry();

//	this->initBarrier.exec();

	delete this;
}


/**** updateProjects ****/

void projectsAddItemFunct ( QComboBox *projects, QJsonObject project )
{
	int projectId = project["id"].toInt();
	QString projectIdentifier = project["identifier"].toString();
	QString projectName       = project["name"].toString();
	QString caption = QString ( "%1: %2" ).arg ( projectIdentifier ).arg ( projectName );
	projects->addItem ( caption, projectId );
	return;
}


bool LogTimeWindow_projectsFilter ( QWidget *__this, QJsonObject item )
{
	LogTimeWindow *_this = reinterpret_cast<LogTimeWindow *> ( __this );
	( void ) _this;
	( void ) item;
	//if (item["name"].toString().indexOf(_this->projectsFilter_namePart, Qt::CaseInsensitive) == -1)
	//    return false;
	int project_id = item["id"].toInt();

	//qDebug("LogTimeWindow_projectsFilter(): %i, %i", project_id, _this->issuesFiltered_byProjectId[project_id].count());
	if ( _this->issuesFiltered_byProjectId[project_id].count() == 0 )
		return false;

	return true;
}


void LogTimeWindow::projects_display()
{
	qDebug ( "LogTimeWindow::projects_display()" );
	this->projectsDisplayMutex.lock();
	/*
	    if (!this->projectsDisplayMutex.tryLock()) {
	        if (!this->projectsDisplayExceptionMutex.tryLock())
	            return;

	        this->projectsDisplayRetryTimer.start(100);

	        this->projectsDisplayExceptionMutex.unlock();
	        return;
	    }*/

	/*
	    this->issues_byProjectId.clear();
	    foreach (const QJsonObject &issue, this->issues.get()) {
	        QJsonObject project    = issue["project"].toObject();
	        int         project_id = project["id"].toInt();
	        this->issues_byProjectId[project_id].append(issue);
	    }*/

	if ( this->projects.filtered.get().count() > 0 ) { // TODO: remove this if. It's added only to forbid updating of the ComboBox due to buggy this process implementation above. The bug is duplicating projects in the ComboBox on each update.
		this->projectsDisplayMutex.unlock();
		return;
	}

	this->projects.filter ( reinterpret_cast<QWidget *> ( this ), LogTimeWindow_projectsFilter );
	this->projects.display ( this->ui->project, reinterpret_cast<QWidget *> ( this ), projectsAddItemFunct );
	this->projectsDisplayMutex.unlock();
}

void LogTimeWindow::get_projects_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg )
{
	( void ) reply;
	( void ) arg;
	this->updateProjectsMutex.lock(); // is not a thread-safe function, locking
	QJsonObject answer   = json->object();
	QJsonArray  projects = answer["projects"].toArray();
	this->projects.set ( projects );
	this->projects_display();
	this->updateProjectsMutex.unlock();
	return;
}

QList<QJsonObject> LogTimeWindow::issues_get_byProjectId ( int project_id )
{
	return this->issues_byProjectId[project_id];
}

int LogTimeWindow::updateProjects()
{
	redmine->get_projects ( this, ( Redmine::callback_t ) &LogTimeWindow::get_projects_callback, NULL );
	return 0;
}

/**** /updateProjects ****/

/**** updateIssues ****/

void LogTimeWindow_issuesWidgetItemSetText ( QWidget *__this, QTreeWidgetItem *widgetItem, QJsonObject issue, RedmineItemTree *tree, int level )
{
	( void ) __this;
	( void ) tree;
	( void ) level;
	widgetItem->setText ( 0, QString::number ( issue["id"].toInt() ) );
	widgetItem->setText ( 1, issue["subject"].toString() );
	return;
}

bool LogTimeWindow_issuesFilter ( QWidget *__this, QJsonObject item )
{
	LogTimeWindow  *_this = reinterpret_cast<LogTimeWindow *> ( __this );
	int             project_id;
	int status_id = item["status"].toObject() ["id"].toInt();

	if ( status_id != 2 /* В НИЯУ МИФИ это статус «Выполняется» */ )
		return false;

	/*
	int assigned_to_id = item["assigned_to"].toObject()["id"].toInt();
	if (assigned_to_id != redmine->me()["id"].toInt())
	    return false;
	*/

	if ( _this->selected_project_id == 0 )
		return true;

	project_id = item["project"].toObject() ["id"].toInt();

	if ( _this->projects.isDescendant ( project_id, _this->selected_project_id ) )
		return true;

	if ( project_id == _this->selected_project_id )
		return true;

	/*


	    if (_this->selected_projects_id.contains(project_id))
	        return true;

	    if (!_this->showProjectIssues_recursive)
	        return false;

	*/
	return false;
}

void LogTimeWindow::issues_display()
{
	qDebug ( "LogTimeWindow::issues_display()" );
	this->projectsDisplayMutex.lock();
	int selected_project_id = this->selected_project_id;
	this->selected_project_id = 0;
	this->issues.filter ( reinterpret_cast<QWidget *> ( this ), LogTimeWindow_issuesFilter );
	this->selected_project_id = selected_project_id;
	this->issuesFiltered_byProjectId.clear();
	foreach ( const QJsonObject & issue, this->issues.filtered.get() ) {
		QJsonObject project = issue["project"].toObject();
		int project_id = project["id"].toInt();
		this->issuesFiltered_byProjectId[project_id].append ( issue );
	}
	this->issues.filter ( reinterpret_cast<QWidget *> ( this ), LogTimeWindow_issuesFilter );
	this->issues.display ( this->ui->issue, reinterpret_cast<QWidget *> ( this ),
	                       LogTimeWindow_issuesWidgetItemSetText );
	this->projectsDisplayMutex.unlock();
	this->projects_display();
	return;
}

void LogTimeWindow::get_issues_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg )
{
	( void ) reply;
	( void ) arg;
	this->updateIssuesMutex.lock(); // is not a thread-safe function, locking
	QJsonObject answer   = json->object();
	QJsonArray  issues = answer["issues"].toArray();
	this->issues.set ( issues );
	this->issues_display();
	this->updateIssuesMutex.unlock();
	return;
}

int LogTimeWindow::updateIssues()
{
	redmine->get_issues ( this, ( Redmine::callback_t ) &LogTimeWindow::get_issues_callback, this, false, "status_id=2&assigned_to_id=me&limit=100" );
	return 0;
}

/**** /updateIssues ****/


void LogTimeWindow::saveCurrentEntry()
{
	int activityId = 0;

	switch ( this->ui->activityType->currentIndex() ) { // This values are from source code of https://tasks.mephi.ru/issues/47429/time_entries/new. You can fix it manually
		case 0:
			activityId = 8;
			break;

		case 1:
			activityId = 9;
			break;

		case 2:
			activityId = 10;
			break;
	}

	QDateTime now = QDateTime::currentDateTime();
	this->ui->untilInput->setDateTime(now);

	this->timeEntry.set ( this->ui->sinceInput->dateTime(), this->ui->untilInput->dateTime(), -1, -1, this->ui->comment->text(), activityId );
	this->timeEntry.save();

	this->ui->sinceInput->setDateTime(now);
}

void LogTimeWindow::on_issue_itemClicked ( QTreeWidgetItem *item, int column )
{
	( void ) item; ( void ) column;

	this->ui->comment->setFocus();
	return;
}

void LogTimeWindow::on_issue_itemSelectionChanged()
{
	this->selected_issues_id.clear();
	foreach ( QTreeWidgetItem * selectedIssueItem, this->ui->issue->selectedItems() ) {
		QJsonObject issue    = this->issues.get ( selectedIssueItem );
		int         issue_id = issue["id"].toInt();
		this->selected_issues_id.insert ( issue_id, issue_id );
	}
	this->timeEntry.setIssueId ( this->selected_issues_id.keys().first() );
	return;
}

void LogTimeWindow::on_project_currentIndexChanged ( int index )
{
	QComboBox *comboBox       = this->ui->project;
	this->selected_project_id = comboBox->itemData ( index ).toInt();
	qDebug ( "this->selected_project_id == %i", this->selected_project_id );

	if ( this->selected_project_id )
		this->timeEntry.setProjectId ( this->selected_project_id );
	else
		this->timeEntry.setProjectId ( redmine->myProject() ["id"].toInt() );

	issues_display();
	return;
}

void LogTimeWindow::on_issue_doubleClicked ( const QModelIndex &index )
{
	( void ) index;

	QDesktopServices::openUrl ( QUrl ( redmine->getUrl ( "issue", this->timeEntry.getIssueId() ) ) );
}

void LogTimeWindow::on_comment_editingFinished()
{
	//this->ui->accept->click();
	this->ui->accept->setFocus();

	return;
}

void LogTimeWindow::on_continousLoggingStartButton_clicked()
{
	this->ui->continousLoggingStartButton->setEnabled(false);

	QDateTime now = QDateTime::currentDateTime();
	this->ui->sinceInput->setDateTime(now);

	this->ui->continousLoggingStopButton->setEnabled(true);
}


void LogTimeWindow::on_continousLoggingStopButton_clicked()
{

	this->ui->continousLoggingStopButton->setEnabled(false);
	this->saveCurrentEntry();
	this->ui->continousLoggingStartButton->setEnabled(true);
}

void LogTimeWindow::on_issue_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	Q_UNUSED(current)
	Q_UNUSED(previous)

	if (this->ui->continousLoggingStopButton->isEnabled()) {
		this->saveCurrentEntry();
	}
}

void LogTimeWindow::on_comment_textEdited(const QString &arg1)
{
	if (this->ui->continousLoggingStopButton->isEnabled()) {
		this->saveCurrentEntry();
	}
}
