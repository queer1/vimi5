/*
 * Application main window class
 * Copyright (C) 2010-2012 cwk <coolwk@gmail.com>
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

#include "aboutdialog.h"
#include "mainwindow.h"
#include "collectionview.h"
#include "covermaker.h"
#include "settingsdialog.h"
#include "tagreplacementdialog.h"
#include "favouritetagdialog.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QApplication>
#include <QThreadPool>
#include <QWaitCondition>
#include <QSettings>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("vimi");
    setWindowIcon(QIcon(":/images/icon.png"));

    CollectionView *cv = new CollectionView;
    setCentralWidget(cv);

    // Set up the File menu
    QMenu *fileMenu = new QMenu("&File", this);
    fileMenu->addAction(QIcon(), "&Rescan...", cv->collection(), SLOT(rescan()), QKeySequence::Refresh);
    fileMenu->addAction(QIcon(), "&Settings...", this, SLOT(showSettings()), QKeySequence::Preferences);
    fileMenu->addAction(QIcon(), "&Set Path to Collection...", this, SLOT(getCollectionPath()));
    fileMenu->addAction(QIcon(), "&Quit", this, SLOT(close()), QKeySequence::Quit);
    menuBar()->addMenu(fileMenu);

    // Set up the Tag menu
    QMenu *tagMenu = new QMenu("T&ags", this);
    tagMenu->addAction(QIcon(), "&Replace tags...", this, SLOT(showTagReplacementDialog()));
    tagMenu->addAction(QIcon(), "&Edit favourite tags...", this, SLOT(showFavouriteTagDialog()));
    menuBar()->addMenu(tagMenu);

    // Set up the Help menu
    QMenu *helpMenu = new QMenu("&Help", this);
    helpMenu->addAction(QIcon(), "&About Vimi...", this, SLOT(showAboutDialog()));
    menuBar()->addMenu(helpMenu);

    if (Config::collectionPath().isEmpty() || !QDir(Config::collectionPath()).isReadable()) {
        getCollectionPath();
        cv->collection()->rescan();
    }

    //statusBar()->showMessage(tr("Ready."));
    connect(cv, SIGNAL(statusUpdated(QString)), statusBar(), SLOT(showMessage(QString)));
    connect(this, SIGNAL(updatedTags()), cv, SLOT(updateTagModel()));

    QMetaObject::invokeMethod(this, "showMaximized");

    QSettings settings("Cheggit", "Vimi");
    restoreGeometry(settings.value("geometry").toByteArray());

    connect(cv->collection(), SIGNAL(scanning(bool)), SLOT(setDisabled(bool)));
}

MainWindow::~MainWindow()
{
    QApplication::quit();
}

void MainWindow::showAboutDialog()
{
    AboutDialog *dialog = new AboutDialog(this);
    dialog->show();
}


void MainWindow::getCollectionPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory Containing Collection"),
                                                    Config::collectionPath(),
                                                    QFileDialog::ShowDirsOnly);
    if (dir != "") {
        Config::setCollectionPath(dir);
    }
}

void MainWindow::showSettings()
{
    SettingsDialog dialog;
    dialog.show();
    dialog.exec();
}

void MainWindow::showTagReplacementDialog()
{
    TagReplacementDialog *dialog = new TagReplacementDialog(this);
    dialog->show();
    connect(dialog, SIGNAL(finished(int)), this, SIGNAL(updatedTags()));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("Cheggit", "Vimi");
    settings.setValue("geometry", saveGeometry());
    QWidget::closeEvent(event);
}

void MainWindow::showFavouriteTagDialog()
{
    FavouriteTagDialog dialog;
    dialog.show();
    dialog.exec();
}
