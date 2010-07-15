# Copyright 2009 cwk
QT += core \
    gui \
    sql \
    webkit \
    network \
    xml
TARGET = vimi
TEMPLATE = app
SOURCES += main.cpp \
    collectionview.cpp \
    videofilterproxymodel.cpp \
    collection.cpp \
    config.cpp \
    infopanel.cpp \
    tagdialog.cpp \
    aboutdialog.cpp \
    cheggitview.cpp \
    mainwindow.cpp
HEADERS += collectionview.h \
    videofilterproxymodel.h \
    collection.h \
    config.h \
    infopanel.h \
    tagdialog.h \
    aboutdialog.h \
    cheggitview.h \
    mainwindow.h
RESOURCES += images.qrc
