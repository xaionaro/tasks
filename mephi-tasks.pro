#-------------------------------------------------
#
# Project created by QtCreator 2015-03-06T18:00:12
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mephi-tasks
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11 -O0 -march=native

SOURCES += main.cpp\
    helpwindow.cpp \
    redmine.cpp \
    syntaxwindow.cpp \
    mainwindow-rector.cpp \
    mainwindow-full.cpp \
    mainwindow-common.cpp

HEADERS  += \
    helpwindow.h \
    redmine.h \
    common.h \
    syntaxwindow.h \
    mainwindow-rector.h \
    mainwindow-full.h \
    mainwindow-common.h

FORMS    += \
    helpwindow.ui \
    syntaxwindow.ui \
    mainwindow-full.ui \
    mainwindow-rector.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/build-qtredmine-Desktop_Qt_5_4_1_MinGW_32bit-Release/release/ -lqtredmine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/build-qtredmine-Desktop_Qt_5_4_1_MinGW_32bit-Debug/debug/ -lqtredmine
else:unix: LIBS += -L$$PWD/build-qtredmine-Desktop-Debug/ -lqtredmine

INCLUDEPATH += $$PWD/qtredmine
DEPENDPATH += $$PWD/qtredmine

OTHER_FILES += \
    images/bad.png \
    images/heart.png \
    images/trash.png

RESOURCES += \
    mephi-tasks.qrc
