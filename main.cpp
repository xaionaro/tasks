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
#include "mainwindow-full.h"
#include "syntaxwindow.h"
#include "common.h"
#include <QApplication>
#include <QSettings>
#include <QDir>

Redmine *redmine = NULL;

struct settings settings;

void loadSettings() {
    QSettings qsettings(settings.settingsFilePath, QSettings::IniFormat);
    settings.apiKey       = qsettings.value("apiKey").toString();
    settings.issuesFilter = qsettings.value("issuesFilter").toString();

    QString mode          = qsettings.value("mode").toString();

    if (mode == "rector") {
        settings.mode = MODE_RECTOR;
    } else {
        settings.mode = MODE_FULL;
    }

    return;
}

void saveSettings() {
    QSettings qsettings(settings.settingsFilePath, QSettings::IniFormat);
    qsettings.setValue("apiKey",        settings.apiKey);
    qsettings.setValue("issuesFilter",  settings.issuesFilter);
    qsettings.setValue("mode",         (settings.mode == MODE_RECTOR) ? "rector" : "full");

    return;
}

int main(int argc, char *argv[])
{
    qDebug("Starting");

    QApplication a(argc, argv);
    QStringList arglst = a.arguments();

    settings.settingsFilePath = QDir::toNativeSeparators( QApplication::applicationDirPath() + "/mephi-tasks.ini" );
    qDebug("Settings path is: %s", settings.settingsFilePath.toUtf8().data());
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

    /* We need to follow the important next order of objects initialization:
     * QApplication, Redmine, MainWindowRector
     */
    {
        Redmine    _redmine;
        redmine = &_redmine;
        redmine->apiKey(settings.apiKey);
        redmine->init();
        switch (settings.mode) {
            case MODE_RECTOR:
                {
                    MainWindowRector w;
                    w.show();

                    a.setQuitOnLastWindowClosed(false);
                    return a.exec();
                }
            default:
                {
                    MainWindowFull w;
                    w.show();

                    //a.setQuitOnLastWindowClosed(false);
                    return a.exec();
                }
        }
    }
}
