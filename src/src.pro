# Copyright 2009 cwk

DEFINES += __STDC_CONSTANT_MACROS

LIBS += -lavformat -lavcodec -lswscale -lavutil

QT += core\
      gui\
      network\
      webkit\
      xml

TARGET = vimi
TEMPLATE = app

DESTDIR = ..

OBJECTS_DIR = ../build
MOC_DIR = ../build
UI_DIR = ../build
RCC_DIR = ../build

SOURCES += main.cpp\
           aboutdialog.cpp\
           collection.cpp\
           collectionview.cpp\
           config.cpp\
           cookiestorage.cpp\
           infopanel.cpp\
           mainwindow.cpp\
           tagdialog.cpp\
           videofilterproxymodel.cpp \
    video.cpp \
    covermaker.cpp \
    videowidget.cpp \
    coverlabel.cpp \
    settingsdialog.cpp \
    coverloader.cpp \
    tagreplacementdialog.cpp \
    favouritetagdialog.cpp

HEADERS += aboutdialog.h\
           collection.h\
           collectionview.h\
           config.h\
           cookiestorage.h\
           infopanel.h\
           mainwindow.h\
           tagdialog.h\
           videofilterproxymodel.h \
    video.h \
    covermaker.h \
    videowidget.h \
    coverlabel.h \
    version.h \
    settingsdialog.h \
    coverloader.h \
    tagreplacementdialog.h \
    favouritetagdialog.h

win32 {
    LIBS += -LC:\\ffmpeg\\lib -static-libgcc
    CONFIG += static
    INCLUDEPATH += C:\\ffmpeg\\include
}

include(images/images.pro)
