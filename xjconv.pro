TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    xmllexer.cpp \
    xmlparser.cpp \
    jsongenerator.cpp

HEADERS += \
    xmllexer.h \
    xmlparser.h \
    element.h \
    xmlerror.h \
    jsongenerator.h

LIBS += -licuio -licuuc
