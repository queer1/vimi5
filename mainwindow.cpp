// Copyright 2009 cwk

#include "config.h"
#include "mainwindow.h"
#include "tagdialog.h"

#include <QFileDialog>
#include <QGroupBox>
#include <QVBoxLayout>
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
    QGroupBox *tagContainer = new QGroupBox(splitter);
    tagContainer->setLayout(new QVBoxLayout);
    tagContainer->setTitle("Tags");
    splitter->addWidget(tagContainer);

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

    // Set up the video list view
    QGroupBox *videoContainer = new QGroupBox(splitter);
    videoContainer->setLayout(new QVBoxLayout);
    videoContainer->setTitle("Videos");;
    splitter->addWidget(videoContainer);

    m_videoFilterEdit = new QLineEdit(this);
    videoContainer->layout()->addWidget(m_videoFilterEdit);

    m_videoView = new QTreeView(this);
    m_videoView->setRootIsDecorated(false);
    videoContainer->layout()->addWidget(m_videoView);

    m_videoModel = new VideoFilterProxyModel;
    m_videoModel->setSourceModel(m_collection);
    m_videoView->setModel(m_videoModel);


    // Set up the info panel
    m_infoPanel = new InfoPanel(this);
    splitter->addWidget(m_infoPanel);

    // Connect signals
    connect(m_tagModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(updateVideoFilter(QStandardItem*)));
    connect(m_collection, SIGNAL(updated()), this, SLOT(updateTagModel()));
    connect(m_videoView, SIGNAL(activated(QModelIndex)), this, SLOT(updateInfoPanel(QModelIndex)));
    connect(m_tagFilterEdit, SIGNAL(textChanged(QString)), m_tagFilterModel, SLOT(setFilterFixedString(QString)));
    connect(m_videoFilterEdit, SIGNAL(textChanged(QString)), m_videoModel, SLOT(setFilterFixedString(QString)));
    connect(m_infoPanel, SIGNAL(editTags()), this, SLOT(editTags()));

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
    query.exec("SELECT DISTINCT name FROM tag");

    m_tagModel->clear();

    QStandardItem *tag;
    while (query.next()) {
        tag = new QStandardItem(query.value(0).toString());
        tag->setCheckable(true);
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

void MainWindow::editTags()
{
    TagDialog dialog(m_infoPanel->videoName(), m_collection, this);
    dialog.show();
    dialog.raise();
    dialog.exec();
    updateTagModel();
}
