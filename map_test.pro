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
    src/mercator.cpp \
    src/kmlwriter.cpp \
    src/gridtraversal.cpp \
    src/test_occupancy.cpp \
    src/test_bresenhem.cpp \
    src/logReader.cpp \
    src/occupancy_predictor.cpp \
    src/occupancy_grid.cpp \
    src/parking_lots.cpp

HEADERS  += \
    headers/window.h \
    headers/renderarea.h \
    headers/datagenerator.h \
    headers/mercator.h \
    headers/kmlwriter.h \
    headers/gridtraversal.h \
    headers/test_bresenhem.h \
    headers/test_occupancy.h \
    headers/point_with_rot.h \
    headers/logReader.h \
    headers/occupancy_predictor.h \
    headers/parking_lots.h \
    headers/occupancy_cell.h \
    headers/occupancy_grid.h

INCLUDEPATH += \
    headers \
    src

FORMS    +=

RESOURCES += \
    temp_maps.qrc

INCLUDEPATH += /usr/include

LIBS += -L/usr/lib -lkmlbase -lkmldom

QT += testlib
