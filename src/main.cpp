/*
 * Main entry point for application
 * Copyright (C) 2009-2012 cwk <coolwk@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <QApplication>
#include <QDebug>
#include <QSplashScreen>
#include <QLabel>
#include <QQuickView>
#include <QQmlContext>
#include <QUrl>

#include <ctime>

#include "collection.h"
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

//    Collection::instance()->rescan();

    qDebug() << QT_VERSION_STR;
    QQuickView view;
    Collection collection;
    view.rootContext()->setContextProperty("videoModel", &collection);
    view.setSource(QUrl::fromLocalFile("../qml/main.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
//    view.showMaximized();
    view.show();

//    view.show();

    /*QSplashScreen splash(QPixmap(":/images/splash.png"));
    splash.showMessage("Loading cache and covers...", Qt::AlignBottom);
    splash.show();
//    for (int i=0;i<10;i++)// Ugly hack, don't know why I need it
        a.processEvents(QEventLoop::WaitForMoreEvents | QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
    MainWindow w;
    splash.finish(&w);
    w.show();

    end = clock();
    qDebug() << "Startup finished in" << ((float) (end - start) / CLOCKS_PER_SEC) << "seconds.";*/

    return a.exec();
}

