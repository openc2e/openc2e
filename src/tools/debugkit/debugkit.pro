CONFIG += qt warn_on debug
QT += network

HEADERS += debugkit.h
SOURCES += debugkit.cpp
SOURCES += main.cpp
LIBS += ../../streamutils.o
INCLUDEPATH += ../..
