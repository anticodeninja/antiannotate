#-------------------------------------------------
#
# Project created by QtCreator 2019-06-02T14:29:26
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = antiannotate
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += LOG_ENGINE LOG_SPECTRUMANALYSER LOG_WAVEFORM PAINT_EVENT_TRACE

SOURCES += \
        main.cpp \
        antiannotate.cpp \
        engine.cpp \
        progressbar.cpp \
        utils.cpp \
        waveform.cpp \
        wavfile.cpp

HEADERS += \
        antiannotate.h \
        engine.h \
        progressbar.h \
        utils.h \
        waveform.h \
        wavfile.h

LIBS += -L$$PWD/../../../big-projects/qt-proj/build-fftreal-Desktop-Release/
LIBS += -lfftreal

INCLUDEPATH += $$PWD/3rdparty/fftreal
