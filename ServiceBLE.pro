QT += core bluetooth
QT -= gui

CONFIG += c++11

TARGET = ServiceBLE
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    CServerBLE.cpp

HEADERS += \
    CServerBLE.h

target.path = /usr/bin

INSTALLS += target
