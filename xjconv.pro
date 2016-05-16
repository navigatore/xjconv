TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    xmllexer.cpp

HEADERS += \
    xmllexer.h

LIBS += -licuio -licuuc
