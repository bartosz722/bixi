######################################################################
# Automatically generated by qmake (3.0) niedz. lip 19 17:42:26 2015
######################################################################

TEMPLATE = app
TARGET = bixi
INCLUDEPATH += . ../PhysicalObjects ../Utilities ../Universe
CONFIG -= qt
CONFIG += c++11
QMAKE_CXXFLAGS += -pthread
LIBS += -pthread

# Input
HEADERS += ../PhysicalObjects/PhysicalObject.h \
           ../PhysicalObjects/PhysicalObjectType.h \
           ../PhysicalObjects/Spacecraft.h \
           ../PhysicalObjects/SphericalObject.h \
           ../Universe/PhysicalObjectsContainer.h \
           ../Universe/Universe.h \
           ../Utilities/Assert.h \
           ../Utilities/BasicDefinitions.h \
           ../Utilities/PhysicalDefinitions.h \
           ../Utilities/Vector.h
SOURCES += ../PhysicalObjects/PhysicalObject.cpp \
           ../PhysicalObjects/Spacecraft.cpp \
           ../PhysicalObjects/SphericalObject.cpp \
           ../Universe/PhysicalObjectsContainer.cpp \
           ../Universe/Universe.cpp \
           ../Utilities/PhysicalDefinitions.cpp \
           ../Utilities/Vector.cpp \
           ../ProjConsole/main.cpp \
           ../ProjConsole/test.cpp