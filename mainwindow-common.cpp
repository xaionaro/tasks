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

#include <QSettings>
#include <QMenu>

#include "mainwindow-common.h"


MainWindowCommon::MainWindowCommon ( QWidget *parent ) :
	QMainWindow ( parent )
{
	QSettings qsettings ( settings.settingsFilePath, QSettings::IniFormat );
	this->sortFunctMap.insert ( SORT_UNDEFINED,           NULL );
	this->sortFunctMap.insert ( SORT_NAME_ASC,            issueCmpFunct_name_lt );
	this->sortFunctMap.insert ( SORT_NAME_DESC,           issueCmpFunct_name_gt );
	this->sortFunctMap.insert ( SORT_ASSIGNEE_ASC,        issueCmpFunct_assignee_lt );
	this->sortFunctMap.insert ( SORT_ASSIGNEE_DESC,       issueCmpFunct_assignee_gt );
	this->sortFunctMap.insert ( SORT_DUE_TO_ASC,          issueCmpFunct_dueTo_lt );
	this->sortFunctMap.insert ( SORT_DUE_TO_DESC,         issueCmpFunct_dueTo_gt );
	this->sortFunctMap.insert ( SORT_STATUS_POS_ASC,      issueCmpFunct_statusPosition_lt );
	this->sortFunctMap.insert ( SORT_STATUS_POS_DESC,     issueCmpFunct_statusPosition_gt );
	this->sortFunctMap.insert ( SORT_UPDATED_ON_ASC,      issueCmpFunct_updatedOn_lt );
	this->sortFunctMap.insert ( SORT_UPDATED_ON_DESC,     issueCmpFunct_updatedOn_gt );
	this->sortFunctMap.insert ( SORT_STATUS_ISCLOSED_ASC, issueCmpFunct_statusIsClosed_lt );

	this->sortFunctMap.insert ( SORT_TIMEENTRY_FROM_ASC,  timeEntryCmpFunct_from_lt );

	memset ( this->sortColumn, 0, sizeof ( this->sortColumn ) );
	this->sortColumn[0] = ( MainWindowCommon::ESortColumn ) qsettings.value ( "sortcode"   ).toInt();
	this->sortLogicalIndex =                                qsettings.value ( "sortcolumn" ).toInt();
	this->sortOrder        =              ( Qt::SortOrder ) qsettings.value ( "sortorder"  ).toInt();

	this->createIconComboBox();
	this->createTrayIcon();
	this->status ( GOOD );
	this->setIcon ( GOOD );
	this->trayIcon->show();

	this->logTimeWindow = NULL;

	return;
}


MainWindowCommon::~MainWindowCommon()
{
	QSettings qsettings ( settings.settingsFilePath, QSettings::IniFormat );
	qsettings.setValue ( "sortcode",   ( int ) this->sortColumn[0] );
	qsettings.setValue ( "sortcolumn", ( int ) this->sortLogicalIndex );
	qsettings.setValue ( "sortorder",  ( int ) this->sortOrder );
	return;
}

/**** updateRoles ****/

void MainWindowCommon::get_roles_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg )
{
	( void ) reply;
	( void ) arg;
	QJsonArray  roles   = json->object() ["roles"].toArray();
	this->roles.set ( roles );
	return;
}

int MainWindowCommon::updateRoles()
{
    redmine->get_roles ( ( Redmine::callback_t ) &MainWindowCommon::get_roles_callback, this );
	return 0;
}

/**** updateMemberships ****/

void MainWindowCommon::get_memberships_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg )
{
	( void ) reply;
	( void ) arg;
	QJsonArray  memberships = json->object() ["memberships"].toArray();
	this->memberships.set ( memberships );
	return;
}

int MainWindowCommon::updateMemberships()
{
    redmine->get_memberships ( ( Redmine::callback_t ) &MainWindowCommon::get_memberships_callback, this );
	return 0;
}

/**** /updateMemberships ****/


/**** updateEnumerations ****/

void MainWindowCommon::get_enumerations_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg )
{
	( void ) reply;
	( void ) arg;
	QJsonArray enumerations = json->object() ["enumerations"].toArray();
	this->enumerations.set ( enumerations );
	return;
}

int MainWindowCommon::updateEnumerations()
{
    redmine->get_enumerations ( ( Redmine::callback_t ) &MainWindowCommon::get_enumerations_callback, this );
	return 0;
}

/**** /updateEnumerations ****/

/**** updateProjects ****/

void MainWindowCommon::projects_display()
{
	qFatal ( "projects_display() is not re-implemented by the derivative object" );
}

void MainWindowCommon::get_projects_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg )
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

QList<QJsonObject> MainWindowCommon::issues_get_byProjectId ( int project_id )
{
	return this->issues_byProjectId[project_id];
}

int MainWindowCommon::updateProjects()
{
    redmine->get_projects ( ( Redmine::callback_t ) &MainWindowCommon::get_projects_callback, this );
	return 0;
}

/**** /updateProjects ****/


/**** updateIssues ****/

void MainWindowCommon::issues_display()
{
	qFatal ( "issues_display() is not re-implemented by the derivative object" );
}

void MainWindowCommon::get_issues_callback ( QNetworkReply *reply, QJsonDocument *json, void *arg )
{
	( void ) reply;
	( void ) arg;
	this->updateIssuesMutex.lock(); // is not a thread-safe function, locking
	QJsonObject answer = json->object();
	QJsonArray  issues = answer["issues"].toArray();
	this->issues.set ( issues );
	this->issues_display();
	this->updateIssuesMutex.unlock();
	return;
}

int MainWindowCommon::updateIssues()
{
    redmine->get_issues ( ( Redmine::callback_t ) &MainWindowCommon::get_issues_callback, this, false, "status=*&limit=10000" );
	return 0;
}

/**** /updateIssues ****/

/**** tray-related stuff ****/

void MainWindowCommon::createTrayIcon()
{
	this->trayIconMenu = new QMenu ( this );
	this->trayIcon = new QSystemTrayIcon ( this );
	this->trayIcon->setContextMenu ( trayIconMenu );

	return;
}

void MainWindowCommon::setIcon ( EIcon index )
{
	//qDebug("icon: %i", index);
	QIcon icon = this->iconComboBox.itemIcon ( index );
	this->trayIcon->setIcon ( icon );
	this->setWindowIcon ( icon );
	this->trayIcon->setToolTip ( this->iconComboBox.itemText ( index ) );
}

void MainWindowCommon::createIconComboBox()
{
	this->iconComboBox.addItem ( QIcon ( ":/images/good.png" ), tr ( "Просроченных задач нет" ) );
	this->iconComboBox.addItem ( QIcon ( ":/images/bad.png" ),  tr ( "Есть просроченные задачи" ) );
	return;
}

void MainWindowCommon::on_closeLogTimeWindow()
{
	this->logTimeWindow = NULL;

	return;
}

void MainWindowCommon::openLogTimeWindow()
{
	if (this->logTimeWindow != NULL)
		delete this->logTimeWindow;
	this->logTimeWindow = new LogTimeWindow;

	connect ( this->logTimeWindow, SIGNAL ( on_destructor() ), this, SLOT ( on_closeLogTimeWindow() ) );

	this->logTimeWindow->show();

	return;
}


void MainWindowCommon::on_closeShowTimeWindow()
{
	this->showTimeWindow = NULL;

	return;
}

void MainWindowCommon::openShowTimeWindow()
{
	if (this->showTimeWindow != NULL)
		delete this->showTimeWindow;
	this->showTimeWindow = new ShowTimeWindow;

	connect ( this->showTimeWindow, SIGNAL ( on_destructor() ), this, SLOT ( on_closeShowTimeWindow() ) );

	this->showTimeWindow->show();

	return;
}


void MainWindowCommon::showOnTop()
{
#ifdef Q_OS_WIN32
	// raise() doesn't work :(
	Qt::WindowFlags flags_old   = this->windowFlags();
	Qt::WindowFlags flags_ontop = flags_old | Qt::WindowStaysOnTopHint;
	this->setWindowFlags ( flags_ontop );
	this->show();
	this->setWindowFlags ( flags_old );
	this->show();
#else
	this->show();
	this->raise();
#endif
	return;
}

void MainWindowCommon::toggleShowHide()
{
	if ( this->isVisible() ) {
		settings.hideOnStart = true;
		this->hide();
	} else {
		settings.hideOnStart = false;
		this->showOnTop();
	}

	return;
}


/**** /tray-related stuff ****/
