// Copyright 2009 cwk

#include <QtGui/QApplication>
#include <QDebug>
#include <QSplashScreen>
#include <QLabel>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Vimi");
    a.setOrganizationName("Cheggit");
    a.setOrganizationDomain("cheggit.net");

    QSplashScreen splash(QPixmap(":/images/splash.png"));
    splash.showMessage("Loading...");
    splash.show();
    a.processEvents();
    MainWindow w;
    splash.finish(&w);
    w.show();
    return a.exec();
}

