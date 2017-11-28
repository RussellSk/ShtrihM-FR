TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


HEADERS += arxserial.h \
    custom.h \
    custompacket.h \
    customcommands.h \
    settings.h

SOURCES += main.cpp \
    arxserial.cpp \
    custom.cpp \
    custompacket.cpp \
    settings.cpp

LIBS += -L/usr/lib/x86_64-linux-gnu/ \
    -lboost_filesystem \
    -lboost_system \
    -lboost_thread \
