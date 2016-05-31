TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    xmllexer.cpp \
    xmlparser.cpp

HEADERS += \
    xmllexer.h \
    xmlparser.h \
    element.h

LIBS += -licuio -licuuc
