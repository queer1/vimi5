# Copyright 2009 cwk
QT += core \
    gui \
    sql
TARGET = vimi
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    videofilterproxymodel.cpp \
    collection.cpp \
    config.cpp \
    infopanel.cpp \
    tagdialog.cpp \
    aboutdialog.cpp
HEADERS += mainwindow.h \
    videofilterproxymodel.h \
    collection.h \
    config.h \
    infopanel.h \
    tagdialog.h \
    aboutdialog.h
