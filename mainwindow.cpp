// Copyright 2009 cwk

#include "config.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSplitter>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    Config::load();

    m_collection = new Collection();

    QSplitter *splitter = new QSplitter(this);
    setCentralWidget(splitter);

    // Set up the tag view
    m_tagView = new QListView(this);
    splitter->addWidget(m_tagView);

    m_tagModel = new QStandardItemModel;
    updateTagModel();
    m_tagView->setModel(m_tagModel);


    // Set up the video list view
    m_videoView = new QTreeView(this);
    m_videoView->setRootIsDecorated(false);
    splitter->addWidget(m_videoView);

    m_videoModel = new VideoFilterProxyModel;
    m_videoModel->setSourceModel(m_collection);
    m_videoView->setModel(m_videoModel);


    // Set up the info panel
    m_infoPanel = new InfoPanel(this);
    splitter->addWidget(m_infoPanel);

    // Connect signals
    connect(m_tagModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(updateTagFilter(QStandardItem*)));
    connect(m_collection, SIGNAL(updated()), this, SLOT(updateTagModel()));
    connect(m_videoView, SIGNAL(activated(QModelIndex)), this, SLOT(updateInfoPanel(QModelIndex)));

    // Set up the menu
    QMenu *fileMenu = new QMenu("&File");
    fileMenu->addAction(QIcon(), "&Rescan...", m_collection, SLOT(rescan()), QKeySequence::Refresh);
    fileMenu->addAction(QIcon(), "&Set Path to Collection...", this, SLOT(getCollectionPath()));
    fileMenu->addAction(QIcon(), "&Quit", this, SLOT(close()), QKeySequence::Quit);
    menuBar()->addMenu(fileMenu);

    m_videoView->resizeColumnToContents(0);
}

MainWindow::~MainWindow()
{
    Config::save();
}

void MainWindow::updateTagModel()
{
    QSqlQuery query;
    query.exec("SELECT name FROM tag");

    m_tagModel->clear();

    QStandardItem *tag;
    while (query.next()) {
        tag = new QStandardItem(query.value(0).toString());
        tag->setCheckable(true);
        tag->setCheckState(Qt::Unchecked);
        m_tagModel->appendRow(tag);
    }
}

void MainWindow::updateTagFilter(QStandardItem *tag)
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

    m_infoPanel->setInfo(videoName,
                         m_collection->getTags(videoName),
                         m_collection->getFiles(videoName),
                         m_collection->getPath(videoName));
}

void MainWindow::getCollectionPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory Containing Collection"),
                                                    Config::collectionPath,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (dir != "")
        Config::collectionPath = dir;
}
