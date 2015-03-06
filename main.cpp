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

    QApplication a(argc, argv);
    QStringList arglst = a.arguments();

    if (arglst.isEmpty()) {
        SyntaxWindow w;
        w.show();
        return a.exec();
    }

    QString apiKey = arglst.first();
    redmine->apiKey(apiKey);
    redmine->init();

    MainWindow w;
    w.show();

    return a.exec();
}
