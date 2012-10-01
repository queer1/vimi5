// Copyright 2009 cwk

#include <QtGui/QApplication>
#include <QDebug>
#include <QSplashScreen>
#include <QLabel>

#include <ctime>

#include "mainwindow.h"
#include "version.h"

int main(int argc, char *argv[])
{
    qDebug() << "Starting...";
    clock_t start, end;
    start = clock();

    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(true);
    a.setApplicationName("Vimi");
    a.setOrganizationName("Cheggit");
    a.setOrganizationDomain("cheggit.net");

    QSplashScreen splash(QPixmap(":/images/splash.png"));
    splash.showMessage("Loading cache and covers...", Qt::AlignBottom);
    splash.show();
//    for (int i=0;i<10;i++)// Ugly hack, don't know why I need it
        a.processEvents(QEventLoop::WaitForMoreEvents | QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
    MainWindow w;
    splash.finish(&w);
    w.show();
    end = clock();
    qDebug() << "Startup finished in" << ((float) (end - start) / CLOCKS_PER_SEC) << "seconds.";
    return a.exec();
}

