#-------------------------------------------------
#
# Project created by QtCreator 2015-03-06T18:00:12
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mephi-tasks
TEMPLATE = app
unix:QMAKE_CXX = ccache g++
gcc:QMAKE_CXXFLAGS += -std=c++11
gcc:QMAKE_CXXFLAGS_RELEASE += -O2 -march=native
gcc:QMAKE_CXXFLAGS_DEBUG += -O0 -ggdb3 #-D__MOBILE__
#win32:gcc:QMAKE_LFLAGS_RELEASE += -static-libgcc -static-libstdc++ -static
#win32:CONFIG += static

SOURCES += main.cpp\
    helpwindow.cpp \
    redmine.cpp \
    syntaxwindow.cpp \
    mainwindow-rector.cpp \
    mainwindow-full.cpp \
    mainwindow-common.cpp \
    htmldelegate.cpp \
    redmineitemtree.cpp \
    redmineitemtreedata.cpp \
    projectmemberswindow.cpp \
    roles.cpp \
    memberships.cpp \
    enumerations.cpp \
    signingwindow.cpp \
    loginwindow.cpp \
    mainwindowandroid.cpp \
    logtimewindow.cpp \
    redmineclass_time_entry.cpp \
    showtimewindow.cpp \
    myqlineedit.cpp \
    planwindow.cpp

HEADERS  += \
    helpwindow.h \
    redmine.h \
    common.h \
    syntaxwindow.h \
    mainwindow-rector.h \
    mainwindow-full.h \
    mainwindow-common.h \
    htmldelegate.h \
    redmineitemtree.h \
    redmineitemtreedata.h \
    projectmemberswindow.h \
    roles.h \
    memberships.h \
    enumerations.h \
    signingwindow.h \
    loginwindow.h \
    mainwindowandroid.h \
    logtimewindow.h \
    redmineclass_time_entry.h \
    showtimewindow.h \
    myqlineedit.h \
    planwindow.h

FORMS    += \
    helpwindow.ui \
    syntaxwindow.ui \
    mainwindow-full.ui \
    mainwindow-rector.ui \
    projectmemberswindow.ui \
    signingwindow.ui \
    loginwindow.ui \
    mainwindowandroid.ui \
    logtimewindow.ui \
    showtimewindow.ui \
    planwindow.ui

     win32:CONFIG(release, debug|release): LIBS += -L$$PWD/build-qtredmine/release/ -lqtredmine
else:win32:CONFIG(debug,   debug|release): LIBS += -L$$PWD/build-qtredmine/debug/   -lqtredmine
else:unix: LIBS += -L$$PWD/build-qtredmine-Desktop/ -lqtredmine

win32:{
    contains(QT_ARCH, i386) {
	LIBS += -LC:/OpenSSL-Win32/lib -llibeay32
	INCLUDEPATH += C:/OpenSSL-Win32/include
    } else {
	LIBS += -LC:/OpenSSL-Win64/lib -llibeay32
	INCLUDEPATH += C:/OpenSSL-Win64/include
    }
}

winrt {
    winphone:equals(WINSDK_VER, 8.0) {
        WINRT_MANIFEST.capabilities += ID_CAP_NETWORKING
    } else {
        WINRT_MANIFEST.capabilities += internetClient
    }
    CONFIG += windeployqt
    QMAKE_CXXFLAGS += -D__WINRT__
}


INCLUDEPATH += $$PWD/qtredmine
DEPENDPATH += $$PWD/qtredmine

OTHER_FILES += \
    images/bad.png \
    images/heart.png \
    images/trash.png

RESOURCES += \
    mephi-tasks.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
