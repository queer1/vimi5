# Copyright 2009 cwk

DEFINES += __STDC_CONSTANT_MACROS
LIBS += -lavformat -lavcodec -lswscale

QT += core\
      gui\
      network\
      webkit\
      xml

TARGET = vimi
TEMPLATE = app

SOURCES += main.cpp\
           aboutdialog.cpp\
           cheggitview.cpp\
           collection.cpp\
           collectionview.cpp\
           config.cpp\
           cookiestorage.cpp\
           infopanel.cpp\
           mainwindow.cpp\
           tagdialog.cpp\
           tagfetchdialog.cpp\
           videofilterproxymodel.cpp \
    video.cpp \
    covermaker.cpp

HEADERS += aboutdialog.h\
           cheggitview.h\
           collection.h\
           collectionview.h\
           config.h\
           cookiestorage.h\
           infopanel.h\
           mainwindow.h\
           tagdialog.h\
           tagfetchdialog.h\
           videofilterproxymodel.h \
    video.h \
    covermaker.h

include(images/images.pro)
