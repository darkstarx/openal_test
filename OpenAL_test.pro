#-------------------------------------------------
#
# Project created by QtCreator 2014-11-05T16:38:24
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenAL_test
TEMPLATE = app


SOURCES += main.cpp\
	mainwindow.cpp \
	SoundManager.cpp \
    Sound.cpp \
    alcheck.cpp \
    SoundBuffer.cpp

HEADERS += mainwindow.hpp \
	SoundManager.hpp \
    log.hpp \
    Sound.hpp \
    types.hpp \
    alcheck.hpp \
    forwards.hpp \
    SoundBuffer.hpp

FORMS += mainwindow.ui

LIBS += -lopenal
LIBS += -lvorbis
LIBS += -lvorbisfile

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic
