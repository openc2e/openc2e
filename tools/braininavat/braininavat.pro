CONFIG += qt warn_on debug

HEADERS += braininavat.h
HEADERS += brainview.h
SOURCES += braininavat.cpp
SOURCES += brainview.cpp
SOURCES += main.cpp
LIBS += ../../Creature_standalone.o ../../genomeFile.o ../../streamutils.o ../../c2eBrain.o ../../Catalogue.o ../../catalogue.lex.o ../../catalogue.tab.o ../../oldBrain.o -lboost_filesystem-mt
INCLUDEPATH += ../..
