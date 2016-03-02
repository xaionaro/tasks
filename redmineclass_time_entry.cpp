#include "redmineclass_time_entry.h"

#include "errno.h"

void RedmineClass_TimeEntry::init()
{
	this->redmine   = NULL;
	this->id        = 0;
	this->saveReply = NULL;
	return;
}

RedmineClass_TimeEntry::RedmineClass_TimeEntry()
{
	this->init();
	return;
}

int RedmineClass_TimeEntry::setRedmine ( Redmine *redmine )
{
	if (this->redmine != NULL) {
		qCritical("This case is not implemented, yet.");
		return ECANCELED;
	}

	this->redmine = redmine;

	connect ( this->redmine, SIGNAL ( callback_call       ( void*, callback_t, QNetworkReply*, QJsonDocument*, void* ) ),
		  this,          SLOT   ( callback_dispatcher ( void*, callback_t, QNetworkReply*, QJsonDocument*, void* ) ) );
	return 0;
}

RedmineClass_TimeEntry::RedmineClass_TimeEntry ( Redmine *redmine )
{
	this->init();
	this->setRedmine ( redmine );
	return;
}

RedmineClass_TimeEntry::~RedmineClass_TimeEntry()
{
	return;
}


int RedmineClass_TimeEntry::setProjectId ( int projectId )
{
	this->projectId = projectId;
	return 0;
}

int RedmineClass_TimeEntry::getProjectId()
{
	return this->projectId;
}

int RedmineClass_TimeEntry::setIssueId ( int issueId )
{
	this->issueId = issueId;
	return 0;
}

int RedmineClass_TimeEntry::getIssueId()
{
	return this->issueId;
}

void RedmineClass_TimeEntry::saveCallback( QNetworkReply *reply, QJsonDocument *timeEntry_doc, void *_null )
{
	( void ) reply; ( void ) timeEntry_doc; ( void ) _null;
	qDebug ( "RedmineClass_TimeEntry::saveCallback()" );

	this->saveTimer.stop();

	QJsonObject timeEntry = timeEntry_doc->object()["time_entry"].toObject();

	if (timeEntry.empty()) {
		qDebug ( "RedmineClass_TimeEntry::saveCallback(): timeEntry.empty()" );
		this->on_saveFailure(reply);
		return;
	}

	int timeEntryId = timeEntry["id"].toInt();

	if (timeEntryId == 0) {
		qDebug ( "RedmineClass_TimeEntry::saveCallback(): timeEntryId == 0" );
		this->on_saveFailure(reply);
		return;
	}

	qDebug ( "RedmineClass_TimeEntry::saveCallback(): ok" );
	this->on_saveSuccess();
	return;
}

void RedmineClass_TimeEntry::saveTimeout()
{
	qDebug ( "RedmineClass_TimeEntry::saveTimeout()" );

	this->on_saveTimeout();
	return;
}

int RedmineClass_TimeEntry::save()
{
	if ( this->redmine == NULL )
		return EHOSTUNREACH;

	if ( this->saveTimer.isActive() ) {
		qDebug("RedmineClass_TimeEntry::save(): Saving is already in progress. Cancelling the new request.");
		return EINPROGRESS;
	}

	QVariantMap timeEntries, timeEntry;
	QString uri;
	RedmineClient::EMode mode = ( this->id != 0 ? RedmineClient::EMode::PUT : RedmineClient::EMode::POST );

	if ( this->id != 0 ) {
		uri = "time_entries/" + QString ( this->id );
		timeEntry["id"]    = QVariant ( this->id );
	} else {
		uri = "time_entries";
	}

	timeEntry["hours"]    = QVariant ( this->hours );
	timeEntry["endtime"]  = QVariant ( this->endtime );

	if ( this->issueId ) {
		timeEntry["issue_id"] = QVariant ( this->issueId );
	} else {
		if ( !this->projectId ) {
			qDebug ( "RedmineClass_TimeEntry::save(): ERROR: this->issueId == 0; this->projectId == 0" );
			return EINVAL;
		}

		timeEntry["project_id"] = QVariant ( this->projectId );
	}

	timeEntry["comments"] = QVariant ( this->comment );
	timeEntry["spent_on"] = QVariant ( this->endtime.date() );
	timeEntry["activity_id"] = QVariant ( this->activityId );
	timeEntries["time_entry"] = timeEntry;
	this->saveReply = this->redmine->request ( mode, "time_entries", this, (Redmine::callback_t)&RedmineClass_TimeEntry::saveCallback, NULL, false, "", timeEntries );

	this->saveTimer.setSingleShot ( true );
	connect ( &this->saveTimer, SIGNAL ( timeout() ), this, SLOT ( saveTimeout() ) );
	this->saveTimer.start ( REDMINE_BASETIMEOUT );

	return 0;
}

int RedmineClass_TimeEntry::set ( QDateTime timeFrom, QDateTime timeTo, int projectId, int issueId, QString comment, int activityId )
{
	this->hours       = ( ( float ) ( ( time_t ) ( timeTo.toTime_t() - timeFrom.toTime_t() ) ) ) / 3600;
	this->endtime     =  timeTo;

	if ( issueId != -1 )
		this->issueId = issueId;

	if ( projectId != -1 )
		this->projectId = projectId;

	this->comment     = comment;
	this->activityId  = activityId;

	if ( issueId == 0 ) {
		qDebug ( "RedmineClass_TimeEntry::set(): issueId is not set. Trying the default." );
		QJsonValueRef defaultIssueIdObj = this->redmine->me() ["default_issue_id"];

		if ( !defaultIssueIdObj.toInt() ) {
			qDebug ( "RedmineClass_TimeEntry::set(): The default issueId is not set, too. ERROR." );
			return EINVAL;
		}

		issueId = defaultIssueIdObj.toInt();
	}

	return 0;
}

/*
int RedmineClass_TimeEntry::set(QJsonObject *json) {
    return 0;
}*/
