#-------------------------------------------------
#
# Project created by QtCreator 2016-03-25T18:17:50
#
#-------------------------------------------------

macx: {
    QMAKE_CXXFLAGS_CXX11 = -std=c++14 -stdlib=libc++
    CONFIG += c++11
} else {
    CONFIG += c++14
}
QT += core gui concurrent
TRANSLATIONS += whsim_pl_PL.ts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = warhammer_battle_simulator
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           simulator.cpp \
           character.cpp \
           diceroller.cpp

HEADERS  += mainwindow.h \
            simulator.h \
            character.h \
            diceroller.h

FORMS    += mainwindow.ui

RESOURCES += resources.qrc
