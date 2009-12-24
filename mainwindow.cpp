// Copyright 2009 cwk

#include "config.h"
#include "mainwindow.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSplitter>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    Config config;

    initializeDatabase();

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

    m_collection = new Collection();
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
    fileMenu->addAction(QIcon(), "&Quit", this, SLOT(close()), QKeySequence::Quit);
    menuBar()->addMenu(fileMenu);

    m_videoView->resizeColumnToContents(0);
}

void MainWindow::initializeDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("vimi.db");
    if (!db.open()) {
        QMessageBox::critical(0, tr("Cannot open database"),
            tr("Unable to establish a database connection.\n"
               "This example needs SQLite support. Please read "
               "the Qt SQL driver documentation for information how "
               "to build it."), QMessageBox::Cancel);
        return;
    }

    // Ensure our tables exist (FIXME: not very elegant, this)
    QSqlQuery query(db);
    query.exec("create table video (name varchar(20), path varchar(200))");
    query.exec("create table tag (name varchar(20))");
    query.exec("create table videoTag (tagId int, videoId int)");

    /* TEST DATA

    query.exec("insert into video values('foo', '/home/w00t/foo')");
    query.exec("insert into video values('bar', '/home/w00t/bar')");

    query.exec("insert into tag values('hawt')");
    query.exec("insert into tag values('not')");
    query.exec("insert into tag values('care')");

    query.exec("insert into videoTag values(1, 2)");
    query.exec("insert into videoTag values(2, 1)");
    query.exec("insert into videoTag values(3, 1)");
    query.exec("insert into videoTag values(3, 2)");*/
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
