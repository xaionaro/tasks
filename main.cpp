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

#ifndef __ANDROID__
//#	define __ANDROID__
#endif

#include "mainwindow-rector.h"
#include "mainwindow-full.h"
#include "mainwindowandroid.h"
#include "loginwindow.h"
#include "syntaxwindow.h"
#include "common.h"
#include <QSettings>
#include <QDir>
#include <QEventLoop>
#include <errno.h>

Redmine      *redmine     = NULL;
QApplication *application = NULL;

struct settings settings;

void loadSettings()
{
	QSettings qsettings ( settings.settingsFilePath, QSettings::IniFormat );
	settings.apiKey       = qsettings.value ( "apiKey" ).toString();
	settings.issuesFilter = qsettings.value ( "issuesFilter" ).toString();
	settings.hideOnStart  = qsettings.value ( "hideOnStart" ).toBool();
	QString mode          = qsettings.value ( "mode" ).toString();

	if ( mode == "rector" ) {
		settings.mode = MODE_RECTOR;
	} else {
		settings.mode = MODE_FULL;
	}

	return;
}


bool issueCmpFunct_statusIsClosed_lt ( const QJsonObject &issue_a, const QJsonObject &issue_b )
{
	int issue_statusIsClosed_a = redmine->get_issue_status ( issue_a["status"].toObject() ["id"].toInt() ) ["is_closed"].toBool();
	int issue_statusIsClosed_b = redmine->get_issue_status ( issue_b["status"].toObject() ["id"].toInt() ) ["is_closed"].toBool();
	return issue_statusIsClosed_a < issue_statusIsClosed_b;
}


bool issueCmpFunct_statusPosition_lt ( const QJsonObject &issue_a, const QJsonObject &issue_b )
{
	int issue_statusPosition_a = redmine->get_issue_status ( issue_a["status"].toObject() ["id"].toInt() ) ["position"].toInt();
	int issue_statusPosition_b = redmine->get_issue_status ( issue_b["status"].toObject() ["id"].toInt() ) ["position"].toInt();
	return issue_statusPosition_a < issue_statusPosition_b;
}
bool issueCmpFunct_statusPosition_gt ( const QJsonObject &issue_a, const QJsonObject &issue_b )
{
	int issue_statusPosition_a = redmine->get_issue_status ( issue_a["status"].toObject() ["id"].toInt() ) ["position"].toInt();
	int issue_statusPosition_b = redmine->get_issue_status ( issue_b["status"].toObject() ["id"].toInt() ) ["position"].toInt();
	return issue_statusPosition_a > issue_statusPosition_b;
}

bool issueCmpFunct_updatedOn_lt ( const QJsonObject &issue_a, const QJsonObject &issue_b )
{
	QString issue_updatedOn_a = issue_a["updated_on"].toString();
	QString issue_updatedOn_b = issue_b["updated_on"].toString();
	return issue_updatedOn_a < issue_updatedOn_b;
}
bool issueCmpFunct_updatedOn_gt ( const QJsonObject &issue_a, const QJsonObject &issue_b )
{
	QString issue_updatedOn_a = issue_a["updated_on"].toString();
	QString issue_updatedOn_b = issue_b["updated_on"].toString();
	return issue_updatedOn_a > issue_updatedOn_b;
}

bool issueCmpFunct_name_lt ( const QJsonObject &issue_a, const QJsonObject &issue_b )
{
	QString issue_name_a = issue_a["name"].toString();
	QString issue_name_b = issue_b["name"].toString();
	return issue_name_a < issue_name_b;
}
bool issueCmpFunct_name_gt ( const QJsonObject &issue_a, const QJsonObject &issue_b )
{
	QString issue_name_a = issue_a["name"].toString();
	QString issue_name_b = issue_b["name"].toString();
	return issue_name_a > issue_name_b;
}

bool issueCmpFunct_assignee_lt ( const QJsonObject &issue_a, const QJsonObject &issue_b )
{
	QString issue_assignee_a = issue_a["assigned_to"].toObject() ["name"].toString();
	QString issue_assignee_b = issue_b["assigned_to"].toObject() ["name"].toString();
	return issue_assignee_a < issue_assignee_b;
}
bool issueCmpFunct_assignee_gt ( const QJsonObject &issue_a, const QJsonObject &issue_b )
{
	QString issue_assignee_a = issue_a["assigned_to"].toObject() ["name"].toString();
	QString issue_assignee_b = issue_b["assigned_to"].toObject() ["name"].toString();
	return issue_assignee_a > issue_assignee_b;
}

bool issueCmpFunct_dueTo_lt ( const QJsonObject &issue_a, const QJsonObject &issue_b )
{
	QString issue_dueTo_a = issue_a["due_date"].toString();
	QString issue_dueTo_b = issue_b["due_date"].toString();
	return issue_dueTo_a < issue_dueTo_b;
}
bool issueCmpFunct_dueTo_gt ( const QJsonObject &issue_a, const QJsonObject &issue_b )
{
	QString issue_dueTo_a = issue_a["due_date"].toString();
	QString issue_dueTo_b = issue_b["due_date"].toString();
	return issue_dueTo_a > issue_dueTo_b;
}

bool timeEntryCmpFunct_from_lt ( const QJsonObject &timeEntry_a, const QJsonObject &timeEntry_b )
{
	QString timeEntry_from_a = timeEntry_a["from"].toString();
	QString timeEntry_from_b = timeEntry_b["from"].toString();
	return timeEntry_from_a < timeEntry_from_b;
}

void saveSettings()
{
	QSettings qsettings ( settings.settingsFilePath, QSettings::IniFormat );
	qsettings.setValue ( "apiKey",        settings.apiKey );
	qsettings.setValue ( "issuesFilter",  settings.issuesFilter );
	qsettings.setValue ( "mode",         ( settings.mode == MODE_RECTOR ) ? "rector" : "full" );
	qsettings.setValue ( "hideOnStart",   settings.hideOnStart );
	return;
}

int main ( int argc, char *argv[] )
{
	qDebug ( "Starting" );
	QApplication a ( argc, argv );
	application = &a;
	QStringList arglst = a.arguments();
	settings.settingsFilePath = QDir::toNativeSeparators ( QApplication::applicationDirPath() + "/mephi-tasks.ini" );
	qDebug ( "Settings path is: %s", settings.settingsFilePath.toUtf8().data() );
	loadSettings();
	/*
	    if (arglst.count() <= 1) {
	        SyntaxWindow synWin;
	        synWin.show();
	        return a.exec();
	    }
	*/

	if ( arglst.count() > 1 )
		settings.apiKey = arglst[1];

	if ( settings.apiKey.length() == 0 ) {
		LoginWindow w;
		w.show();
		w.exec();
		settings.apiKey = w.resultApiKey;
	}

	if ( settings.apiKey.length() == 0 ) {
		qDebug ( "apiKey/password is not set or wrong" );
		return EINVAL;
	}

	/* We need to follow the important next order of objects initialization:
	 * QApplication, Redmine, MainWindowRector
	 */
	{
		int rc = EINVAL;
		Redmine    _redmine;
		redmine = &_redmine;
		redmine->apiKey ( settings.apiKey );
		redmine->init();
#ifdef __ANDROID__
        qDebug("Mode: ANDROID");
		MainWindowAndroid w;
		w.show();
		rc = a.exec();
#else

		switch ( settings.mode ) {
			case MODE_RECTOR: {
					MainWindowRector w;
                    qDebug("Mode: RECTOR");
                    if (!settings.hideOnStart)
						w.show();
					a.setQuitOnLastWindowClosed ( false );
					rc = a.exec();
					break;
				}

			default: {
					MainWindowFull w;
                    qDebug("Mode: FULL");
                    if (!settings.hideOnStart)
						w.show();
					a.setQuitOnLastWindowClosed ( false );
					rc = a.exec();
					break;
				}
		}

#endif
		saveSettings();
		//redmine->cacheSave();
		return rc;
	}
}
