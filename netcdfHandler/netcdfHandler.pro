#-------------------------------------------------
#
# CRITERIA3D distribution
# netCDF handler library
#
#-------------------------------------------------

QT       -= core gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

DEFINES += NETCDF_HANDLER_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/netcdfHandler
    } else {
        TARGET = release/netcdfHandler
    }
}
win32:{
    TARGET = netcdfHandler
}


INCLUDEPATH += ../mathFunctions ../crit3dDate ../gis
INCLUDEPATH += $$(NC4_INSTALL_DIR)/include

SOURCES += \
    netcdfHandler.cpp

HEADERS += \
    netcdfHandler.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
