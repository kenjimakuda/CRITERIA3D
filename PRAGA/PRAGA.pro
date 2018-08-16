#-----------------------------------------------------
#
# PRAGA
# PRogram for AGrometeorological Analysis
#
# this software is part of CRITERIA3D distribution
#
#-----------------------------------------------------

QT       += core gui widgets network sql xml

TARGET = PRAGA
TEMPLATE = app

INCLUDEPATH +=  ../MapGraphics ../soilFluxes3D/header \
                ../crit3dDate ../mathFunctions ../soil ../crop \
                ../meteo ../gis ../utilities \
                ../interpolation ../solarRadiation ../criteria3D \
                ../dbMeteoPoints ../dbMeteoGrid ../climate ../netcdfHandler

CONFIG += debug_and_release

unix:{
    LIBS += -L../MapGraphics/release -lMapGraphics
}
win32:{
    CONFIG(debug, debug|release) {
         LIBS += -L../MapGraphics/debug -lMapGraphics
    } else {
        LIBS += -L../MapGraphics/release -lMapGraphics
    }
}

CONFIG(debug, debug|release) {
    LIBS += -L../netcdfHandler/debug -lnetcdfHandler
    LIBS += -L../climate/debug -lclimate
    LIBS += -L../dbMeteoGrid/debug -ldbMeteoGrid
    LIBS += -L../dbMeteoPoints/debug -ldbMeteoPoints
    LIBS += -L../utilities/debug -lutilities
    LIBS += -L../criteria3D/debug -lcriteria3D
    LIBS += -L../solarRadiation/debug -lsolarRadiation
    LIBS += -L../interpolation/debug -linterpolation
    LIBS += -L../meteo/debug -lmeteo
    LIBS += -L../gis/debug -lgis
    LIBS += -L../crop/debug -lcrop
    LIBS += -L../soilFluxes3D/debug -lsoilFluxes3D
    LIBS += -L../soil/debug -lsoil
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../mathFunctions/debug -lmathFunctions
} else {
    LIBS += -L../netcdfHandler/release -lnetcdfHandler
    LIBS += -L../climate/release -lclimate
    LIBS += -L../dbMeteoGrid/release -ldbMeteoGrid
    LIBS += -L../dbMeteoPoints/release -ldbMeteoPoints
    LIBS += -L../utilities/release -lutilities
    LIBS += -L../criteria3D/release -lcriteria3D
    LIBS += -L../solarRadiation/release -lsolarRadiation
    LIBS += -L../interpolation/release -linterpolation
    LIBS += -L../meteo/release -lmeteo
    LIBS += -L../gis/release -lgis
    LIBS += -L../crop/release -lcrop
    LIBS += -L../soilFluxes3D/release -lsoilFluxes3D
    LIBS += -L../soil/release -lsoil
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../mathFunctions/release -lmathFunctions
}


#LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf -lhdf5
LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf


SOURCES += main.cpp\
    mainwindow.cpp \
    rasterObject.cpp \
    colorlegend.cpp \
    project.cpp \
    stationMarker.cpp \
    rubberBand.cpp \
    dialogWindows.cpp \
    formRunInfo.cpp \
    computationDialog.cpp \
    anomalyLayout.cpp

HEADERS  += mainwindow.h \
    rasterObject.h \
    colorlegend.h \
    project.h \
    guiConfiguration.h \
    stationMarker.h \
    rubberBand.h \
    dialogWindows.h \
    formRunInfo.h \
    computationDialog.h \
    anomalyLayout.h

FORMS    += mainwindow.ui \
    formInfo.ui

