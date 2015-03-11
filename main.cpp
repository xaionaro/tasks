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

#include "mainwindow.h"
#include "syntaxwindow.h"
#include "common.h"
#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDir>

Redmine *redmine = NULL;

struct settings settings;

void loadSettings() {
    QSettings qsettings(settings.settingsFilePath, QSettings::NativeFormat);
    settings.apiKey       = qsettings.value("apiKey").toString();
    settings.issuesFilter = qsettings.value("issuesFilter").toString();

    return;
}

void saveSettings() {
    QSettings qsettings(settings.settingsFilePath, QSettings::NativeFormat);
    qsettings.setValue("apiKey",       settings.apiKey);
    qsettings.setValue("issuesFilter", settings.issuesFilter);

    return;
}

int main(int argc, char *argv[])
{
    Redmine    _redmine;
    redmine = &_redmine;
    qDebug("Starting");

    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append(QFileInfo(argv[0]).dir().path());
    QCoreApplication::setLibraryPaths(paths);

    QApplication a(argc, argv);
    QStringList arglst = a.arguments();

    settings.settingsFilePath = QDir::fromNativeSeparators( QApplication::applicationDirPath() + "/mephi-tasks.ini" );
    loadSettings();
/*
    if (arglst.count() <= 1) {
        SyntaxWindow synWin;
        synWin.show();
        return a.exec();
    }
*/

    if (arglst.count() > 1)
        settings.apiKey = arglst[1];

    redmine->apiKey(settings.apiKey);
    redmine->init();

    MainWindow w;
    w.show();

    a.setQuitOnLastWindowClosed(false);
    return a.exec();
}
