#-------------------------------------------------
#
# Project created by QtCreator 2015-03-06T18:00:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mephi-tasks
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    helpwindow.cpp \
    redmine.cpp \
    syntaxwindow.cpp

HEADERS  += mainwindow.h \
    helpwindow.h \
    redmine.h \
    common.h \
    syntaxwindow.h

FORMS    += mainwindow.ui \
    helpwindow.ui \
    syntaxwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-qtredmine-Desktop-Debug/release/ -lqtredmine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-qtredmine-Desktop-Debug/debug/ -lqtredmine
else:unix: LIBS += -L$$PWD/../build-qtredmine-Desktop-Debug/ -lqtredmine

INCLUDEPATH += $$PWD/qtredmine
DEPENDPATH += $$PWD/qtredmine
