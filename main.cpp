#include "mainwindow.h"
#include "syntaxwindow.h"
#include "common.h"
#include <QApplication>


Redmine *redmine = NULL;
QString issues_filter = "";

int main(int argc, char *argv[])
{
    Redmine    _redmine;
    redmine = &_redmine;
    qDebug("Starting");

    QApplication a(argc, argv);
    QStringList arglst = a.arguments();

    if (arglst.count() <= 1) {
        SyntaxWindow synWin;
        synWin.show();
        return a.exec();
    }

    QString apiKey = arglst[1];
    redmine->apiKey(apiKey);
    redmine->init();

    MainWindow w;
    w.show();

    a.setQuitOnLastWindowClosed(false);
    return a.exec();
}
