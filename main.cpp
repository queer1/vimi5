// Copyright 2009 cwk

#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Vimi");
    a.setOrganizationName("Cheggit");
    a.setOrganizationDomain("cheggit.net");
    MainWindow w;
    w.show();
    return a.exec();
}

