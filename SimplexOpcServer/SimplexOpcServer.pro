QT += core gui widgets

CONFIG += c++11
CONFIG -= app_bundle

TARGET = SimplexOpcServer

TEMPLATE = app

INCLUDEPATH += $$PWD/

SOURCES += main.cpp

SOURCES += SimplexOpcServer.cpp
HEADERS += SimplexOpcServer.h

HEADERS += temperaturesensor.h
SOURCES += temperaturesensor.cpp

HEADERS += customvar.h
SOURCES += customvar.cpp

FORMS   += SimplexOpcServer.ui

include($$PWD/../src/wrapper/quaserver.pri)
include($$PWD/../src/helper/add_qt_path_win.pri)
