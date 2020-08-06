#-------------------------------------------------
#
# Project created by QtCreator 2012-04-20T19:00:32
#
#-------------------------------------------------

QT       += core gui

TARGET = q_image_retrieval
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ImgViewer.cpp

HEADERS  += mainwindow.h \
    ColorSpace.h \
    ImgViewer.h \
    ImgHistogram.h \
    PearsonCorr.h

RESOURCES += \
    res.qrc
