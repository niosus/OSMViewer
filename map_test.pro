#-------------------------------------------------
#
# Project created by QtCreator 2013-09-19T16:48:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = map_test
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += src/main.cpp \
    src/window.cpp \
    src/renderarea.cpp \
    src/datagenerator.cpp \
    src/mercator.cpp

HEADERS  += \
    headers/window.h \
    headers/renderarea.h \
    headers/datagenerator.h \
    headers/mercator.h \
    headers/point_with_rot.h

INCLUDEPATH += \
    headers \
    src

FORMS    +=

RESOURCES += \
    temp_maps.qrc

INCLUDEPATH += /usr/include

LIBS += -L/usr/lib -lkmlbase -lkmldom

QT += testlib
