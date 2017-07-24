
QT  += core xml
QT  -= gui

TARGET = weatherGenerator
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../crit3dDate ../mathFunctions

SOURCES += \
    timeUtility.cpp \
    parserXML.cpp \
    wgClimate.cpp \
    fileUtility.cpp \
    weatherGenerator.cpp

HEADERS += \
    timeUtility.h \
    parserXML.h \
    wgClimate.h \
    fileUtility.h \
    weatherGenerator.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
