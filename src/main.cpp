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


#include <QGuiApplication>
#include <QDebug>
#include <QQuickView>
#include <QQmlContext>
#include <QUrl>

#include <ctime>

#include "collection.h"
#include "config.h"
#include "version.h"
#include "videoframedumper.h"

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    QGuiApplication::setQuitOnLastWindowClosed(true);
    a.setApplicationName("Vimi");
    a.setOrganizationName("Cheggit");
    a.setOrganizationDomain("cheggit.net");


    QQuickView view;
    QObject::connect((QObject*)view.engine(), SIGNAL(quit()), &a, SLOT(quit()));

    Collection collection;
    view.rootContext()->setContextProperty("videoModel", &collection);
    view.rootContext()->setContextProperty("mainWindow", &view);
    view.rootContext()->setContextProperty("config", Config::instance());
    view.setSource(QUrl("qrc:/qml/main.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    if (Config::instance()->fullscreen())
        view.showFullScreen();
    else
        view.showMaximized();

    return a.exec();
}

