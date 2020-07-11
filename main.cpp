/*
    tasks — a Qt-based client to an ITS

    Copyright (C) 2015-2020  Dmitrii Okunev <xaionaro@dx.center>

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

#include "mainwindow-full.h"
#include "mainwindow-android.h"
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
    settings.redmineURL   = qsettings.value ( "redmineURL" ).toString();
	settings.apiKey       = qsettings.value ( "apiKey" ).toString();
	settings.issuesFilter = qsettings.value ( "issuesFilter" ).toString();
	settings.hideOnStart  = qsettings.value ( "hideOnStart" ).toBool();
	QString mode          = qsettings.value ( "mode" ).toString();

    settings.mode = MODE_FULL;

	return;
}

void saveSettings()
{
	QSettings qsettings ( settings.settingsFilePath, QSettings::IniFormat );
	qsettings.setValue ( "apiKey",        settings.apiKey );
	qsettings.setValue ( "issuesFilter",  settings.issuesFilter );
    qsettings.setValue ( "mode",          "full" );
	qsettings.setValue ( "hideOnStart",   settings.hideOnStart );
	return;
}

int main ( int argc, char *argv[] )
{
	qDebug ( "Starting" );
	QApplication a ( argc, argv );
	application = &a;
	QStringList arglst = a.arguments();
    settings.settingsFilePath = QDir::toNativeSeparators ( QApplication::applicationDirPath() + "/tasks.ini" );
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
        redmine = new Redmine(settings.redmineURL);
		redmine->apiKey ( settings.apiKey );
		redmine->init();
#ifdef __MOBILE__
		qDebug("Mode: MOBILE");
		MainWindowAndroid w;
		w.show();
		rc = a.exec();
#else

		switch ( settings.mode ) {
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
