// Copyright 2009 cwk

#include "aboutdialog.h"
#include "config.h"
#include "mainwindow.h"
#include "tagdialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSplitter>
#include <QSqlError>
#include <QStatusBar>
#include <QFileSystemModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    Config::load();

    m_collection = new Collection();

    // Set up the video list view
    QGroupBox *videoContainer = new QGroupBox(this);
    videoContainer->setLayout(new QVBoxLayout);
    videoContainer->setTitle("Videos");

    m_videoFilterEdit = new QLineEdit(this);
    videoContainer->layout()->addWidget(m_videoFilterEdit);

    m_videoView = new QTreeView(this);
    //m_videoView->setRootIsDecorated(false);
    videoContainer->layout()->addWidget(m_videoView);

    m_videoModel = new VideoFilterProxyModel;
    m_videoModel->setSourceModel(m_collection);
    m_videoView->setModel(m_videoModel);


    // Set up the tag view
    QGroupBox *tagContainer = new QGroupBox(this);
    tagContainer->setLayout(new QVBoxLayout);
    tagContainer->setTitle("Tags");
    tagContainer->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    m_tagFilterEdit = new QLineEdit(this);
    tagContainer->layout()->addWidget(m_tagFilterEdit);

    m_tagView = new QListView(this);
    tagContainer->layout()->addWidget(m_tagView);

    m_tagModel = new QStandardItemModel(this);
    m_tagFilterModel = new QSortFilterProxyModel;
    m_tagFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    updateTagModel();
    m_tagFilterModel->setSourceModel(m_tagModel);
    m_tagView->setModel(m_tagFilterModel);


    // Set up the info panel
    m_infoPanel = new InfoPanel(this);

    // Set a splitter as main widget in the window
    QSplitter *splitter = new QSplitter(this);
    setCentralWidget(splitter);

    // Add the widgets to the window
    splitter->addWidget(tagContainer);
    splitter->addWidget(videoContainer);
    splitter->addWidget(m_infoPanel);

    // Connect signals
    connect(m_tagModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(updateVideoFilter(QStandardItem*)));
    connect(m_collection, SIGNAL(updated()), this, SLOT(updateTagModel()));
    connect(m_collection, SIGNAL(statusUpdated(QString)), statusBar(), SLOT(showMessage(QString)));
    connect(m_videoView, SIGNAL(activated(QModelIndex)), this, SLOT(updateInfoPanel(QModelIndex)));
    connect(m_tagFilterEdit, SIGNAL(textChanged(QString)), m_tagFilterModel, SLOT(setFilterFixedString(QString)));
    connect(m_videoFilterEdit, SIGNAL(textChanged(QString)), m_videoModel, SLOT(setFilterFixedString(QString)));
    connect(m_infoPanel, SIGNAL(editTags()), this, SLOT(editTags()));

    // Set up the File menu
    QMenu *fileMenu = new QMenu("&File", this);
    fileMenu->addAction(QIcon(), "&Rescan...", m_collection, SLOT(rescan()), QKeySequence::Refresh);
    fileMenu->addAction(QIcon(), "&Set Path to Collection...", this, SLOT(getCollectionPath()));
    fileMenu->addAction(QIcon(), "&Quit", this, SLOT(close()), QKeySequence::Quit);
    menuBar()->addMenu(fileMenu);

    // Set up the Help menu
    QMenu *helpMenu = new QMenu("&Help", this);
    helpMenu->addAction(QIcon(), "&About Vimi...", this, SLOT(showAboutDialog()));
    menuBar()->addMenu(helpMenu);

    m_videoView->resizeColumnToContents(0);

    statusBar()->showMessage(tr("Ready."));
}

MainWindow::~MainWindow()
{
    Config::save(); // Just in case
}

void MainWindow::updateTagModel()
{
    QSqlQuery query;
    query.exec("SELECT DISTINCT name FROM tag");

    m_tagModel->clear();

    QStringList activeTags = m_videoModel->activeTagFilters();
    m_videoModel->clearTags();

    QStandardItem *tag;
    QString tagName;
    while (query.next()) {
        tagName = query.value(0).toString();
        tag = new QStandardItem(tagName);
        tag->setCheckable(true);

        if (activeTags.contains(tagName)) {
            tag->setCheckState(Qt::Checked);
            m_videoModel->addTag(tagName);
        } else
            tag->setCheckState(Qt::Unchecked);

        m_tagModel->appendRow(tag);
    }
}

void MainWindow::updateVideoFilter(QStandardItem *tag)
{
    if (tag->checkState() == Qt::Unchecked) // Deselected tag
        m_videoModel->removeTag(tag->text());
    else if (tag->checkState() == Qt::Checked)
        m_videoModel->addTag(tag->text());
}

void MainWindow::updateInfoPanel(const QModelIndex &i)
{
    QModelIndex index = i;

    if (index.column() != 0)
        index = index.model()->index(index.row(), 0);

    QString videoName = index.model()->data(index).toString();

    m_infoPanel->setInfo(videoName);
}

void MainWindow::getCollectionPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory Containing Collection"),
                                                    Config::collectionPath,
                                                    QFileDialog::ShowDirsOnly);
    if (dir != "") {
        Config::collectionPath = dir;
        Config::save();
    }
}

void MainWindow::editTags()
{
    TagDialog dialog(m_infoPanel->videoName(), m_collection, this);
    dialog.show();
    dialog.raise();
    dialog.exec();
    updateTagModel();
}

void MainWindow::showAboutDialog()
{
    AboutDialog *dialog = new AboutDialog(this);
    dialog->show();
}
