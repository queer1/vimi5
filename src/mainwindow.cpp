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
#include "covermaker.h"
#include "settingsdialog.h"
#include "tagreplacementdialog.h"
#include "favouritetagdialog.h"
#include "infopanel.h"
#include "tagdialog.h"
#include "videofilterproxymodel.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QApplication>
#include <QThreadPool>
#include <QWaitCondition>
#include <QSettings>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QSplitter>
#include <QSystemTrayIcon>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("vimi");
    setWindowIcon(QIcon(":/images/icon.png"));

    if (Collection::instance()->rowCount() == 0) {
        if (Config::collectionPath() == "") {
            getCollectionPath();
        }
    }
    //QMetaObject::invokeMethod(m_collection, SLOT(rescan()));

    // Set up the video list view
    QGroupBox *videoContainer = new QGroupBox(this);
    videoContainer->setLayout(new QVBoxLayout());
    //videoContainer->setTitle("Videos");

    m_videoFilterEdit = new QLineEdit(this);
    videoContainer->layout()->addWidget(m_videoFilterEdit);

    m_videoView = new QTreeView(this);
    m_videoView->setWordWrap(true);
    m_videoView->setAlternatingRowColors(true);
    m_videoView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_videoView->setSortingEnabled(true);
    m_videoView->sortByColumn(1, Qt::AscendingOrder);

    videoContainer->layout()->addWidget(m_videoView);

    m_videoModel = new VideoFilterProxyModel(this);
    m_videoModel->setSourceModel(Collection::instance());
    m_videoView->setModel(m_videoModel);

    // Set up the tag view
    QGroupBox *tagContainer = new QGroupBox(this);
    tagContainer->setLayout(new QVBoxLayout);
    //tagContainer->setTitle("Tags");

    // Add the favourite tags view
    m_favouriteTags = new QListView();
    tagContainer->layout()->addWidget(m_favouriteTags);
    m_favouriteTagModel = new QStandardItemModel(this);
    m_favouriteTags->setModel(m_favouriteTagModel);

    // The tag filtering line edit
    m_tagFilterEdit = new QLineEdit(this);
    tagContainer->layout()->addWidget(m_tagFilterEdit);


    // The full list of tags
    m_tagView = new QListView(this);
    tagContainer->layout()->addWidget(m_tagView);
    m_tagModel = new QStandardItemModel(this);
    m_tagFilterModel = new QSortFilterProxyModel(this);
    m_tagFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    updateTagModel();
    m_tagFilterModel->setSourceModel(m_tagModel);
    m_tagView->setModel(m_tagFilterModel);



    // Set up the info panel
    m_infoPanel = new InfoPanel(this);

    // Add the widgets to the window

    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(tagContainer);
    splitter->addWidget(videoContainer);
    splitter->addWidget(m_infoPanel);
    setCentralWidget(splitter);

    // Connect signals
    connect(m_tagModel, SIGNAL(itemChanged(QStandardItem*)), SLOT(updateVideoFilter(QStandardItem*)));
    connect(m_favouriteTagModel, SIGNAL(itemChanged(QStandardItem*)), SLOT(updateVideoFilter(QStandardItem*)));
    connect(Collection::instance(), SIGNAL(updated()), SLOT(updateTagModel()));
    connect(m_videoView, SIGNAL(activated(QModelIndex)), SLOT(updateInfoPanel(QModelIndex)));
    connect(m_videoView, SIGNAL(clicked(QModelIndex)), SLOT(updateInfoPanel(QModelIndex)));
    connect(m_tagFilterEdit, SIGNAL(textChanged(QString)), m_tagFilterModel, SLOT(setFilterFixedString(QString)));
    connect(m_videoFilterEdit, SIGNAL(textChanged(QString)), m_videoModel, SLOT(setFilterFixedString(QString)));
    connect(m_infoPanel, SIGNAL(editTags()), SLOT(editTags()));
    connect(m_infoPanel, SIGNAL(createCovers()), SLOT(createCovers()));
    connect(Collection::instance(), SIGNAL(statusUpdated(QString)), SIGNAL(statusUpdated(QString)));
    connect(m_infoPanel, SIGNAL(selectedTag(QString)), SLOT(selectTag(QString)));
    connect(Collection::instance(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), m_videoView, SLOT(repaint()));

    m_videoView->resizeColumnToContents(0);
    //m_videoView->resizeColumnToContents(1);

    m_tagFilterEdit->setPlaceholderText("Search tags");
    m_videoFilterEdit->setPlaceholderText("Search videos");

    splitter->setStretchFactor(1, 2);

    QTimer::singleShot(0, m_videoFilterEdit, SLOT(setFocus()));


    // Set up the File menu
    QMenu *fileMenu = new QMenu("&File", this);
    fileMenu->addAction(QIcon(), "&Rescan...", Collection::instance(), SLOT(rescan()), QKeySequence::Refresh);
    fileMenu->addAction(QIcon(), "&Settings...", this, SLOT(showSettings()), QKeySequence::Preferences);
    fileMenu->addAction(QIcon(), "&Set Path to Collection...", this, SLOT(getCollectionPath()));
    fileMenu->addAction(QIcon(), "&Quit", qApp, SLOT(quit()), QKeySequence::Quit);
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
        Collection::instance()->rescan();
    }

    statusBar()->showMessage(tr("Ready."));
    connect(Collection::instance(), SIGNAL(statusUpdated(QString)), statusBar(), SLOT(showMessage(QString)));
    connect(this, SIGNAL(updatedTags()), Collection::instance(), SLOT(updateTagModel()));

    QMetaObject::invokeMethod(this, "showMaximized");

    QSettings settings("Cheggit", "Vimi");
    restoreGeometry(settings.value("geometry").toByteArray());

    connect(Collection::instance(), SIGNAL(scanning(bool)), SLOT(setDisabled(bool)));

    m_trayIcon = new QSystemTrayIcon(this);
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(trayClicked()));
    m_trayIcon->setIcon(QIcon(":/images/icon.png"));
    m_trayIcon->show();
    m_trayIcon->setContextMenu(fileMenu);
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

void MainWindow::updateTagModel()
{
    m_tagModel->clear();

    QStringList activeTags = m_videoModel->activeTagFilters();
    QStringList allTags = Collection::getTags();
    m_videoModel->clearTags();

    QStandardItem *tag;
    foreach(const QString &tagName, allTags) {
        tag = new QStandardItem(tagName);
        tag->setCheckable(true);

        if (activeTags.contains(tagName)) {
            tag->setCheckState(Qt::Checked);
            m_videoModel->addTag(tagName);
        } else
            tag->setCheckState(Qt::Unchecked);
        m_tagModel->appendRow(tag);
    }
    Collection::instance()->writeCache();

    // Update favourite tag view
    m_favouriteTagModel->clear();

    foreach(const QString &tagName, Config::favouriteTags()) {
        tag = new QStandardItem(tagName);
        tag->setCheckable(true);

        if (activeTags.contains(tagName))
            tag->setCheckState(Qt::Checked);
        else
            tag->setCheckState(Qt::Unchecked);

        m_favouriteTagModel->appendRow(tag);
    }
}


void MainWindow::updateVideoFilter(QStandardItem *tag)
{
    if (tag->checkState() == Qt::Unchecked) // Deselected tag
        m_videoModel->removeTag(tag->text());
    else if (tag->checkState() == Qt::Checked)
        m_videoModel->addTag(tag->text());

    updateInfoPanel(m_videoView->currentIndex());
    m_videoView->scrollTo(m_videoView->currentIndex());
}

void MainWindow::updateInfoPanel(const QModelIndex &i)
{
    if (!i.isValid()) return;

    QModelIndex index = i;

    if (index.column() != 1)
        index = index.model()->index(index.row(), 1);

    QString videoName = index.model()->data(index).toString();

    m_infoPanel->setInfo(videoName);
}

void MainWindow::editTags()
{
    TagDialog dialog(m_infoPanel->videoName());
    dialog.show();
    dialog.raise();
    dialog.exec();
    updateTagModel();
    updateInfoPanel(m_videoView->currentIndex());
}

void MainWindow::createCovers()
{
    CoverMaker dialog(m_infoPanel->videoName(), this);
    connect(&dialog, SIGNAL(coverCreated(QString)), Collection::instance(), SLOT(scanForCovers(QString)));
    dialog.show();
    dialog.raise();
    dialog.exec();

    m_videoView->repaint();
    m_infoPanel->setInfo(m_infoPanel->videoName());
}

void MainWindow::selectTag(const QString &tag)
{
    for (int i=0; i<m_tagModel->rowCount(); i++) {
        if (m_tagModel->item(i)->text() == tag) {
            m_tagModel->item(i)->setCheckState(Qt::Checked);
            break;
         }
    }
    m_videoModel->addTag(tag);
    updateInfoPanel(m_videoView->currentIndex());
    m_videoView->scrollTo(m_videoView->currentIndex());
}

void MainWindow::trayClicked()
{
    if (isHidden()) {
        show();
        raise();
    } else {
        hide();
    }

}
