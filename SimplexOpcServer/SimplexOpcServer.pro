QT += core gui widgets

CONFIG += c++11

TARGET = SimplexOpcServer
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD/

SOURCES += main.cpp

SOURCES += SimplexOpcServer.cpp
HEADERS += SimplexOpcServer.h

HEADERS += temperaturesensor.h
SOURCES += temperaturesensor.cpp

FORMS   += SimplexOpcServer.ui

include($$PWD/../src/wrapper/quaserver.pri)
include($$PWD/../src/helper/add_qt_path_win.pri)
