// Copyright 2009 cwk

#include "config.h"
#include "collectionview.h"
#include "tagdialog.h"
#include "tagfetchdialog.h"
#include "covermaker.h"
#include "mainwindow.h"

#include <QDebug>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QFileSystemModel>
#include <QTimer>
#include <QApplication>

CollectionView::CollectionView(MainWindow *parent) :
    QSplitter(parent)
{
    m_collection = new Collection();
    if (m_collection->rowCount() == 0) {
        if (Config::collectionPath() == "") {
            parent->getCollectionPath();
        }
    }
    m_collection->rescan();

    // Set up the video list view
    QGroupBox *videoContainer = new QGroupBox(this);
    videoContainer->setLayout(new QVBoxLayout);
    //videoContainer->setTitle("Videos");

    m_videoFilterEdit = new QLineEdit(this);
    m_videoFilterEdit->setPlaceholderText("Search videos");
    videoContainer->layout()->addWidget(m_videoFilterEdit);

    m_videoView = new QTreeView(this);
    m_videoView->setWordWrap(true);
    m_videoView->setAlternatingRowColors(true);
    m_videoView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_videoView->setSortingEnabled(true);
    m_videoView->sortByColumn(1, Qt::AscendingOrder);
    videoContainer->layout()->addWidget(m_videoView);

    m_videoModel = new VideoFilterProxyModel;
    m_videoModel->setSourceModel(m_collection);
    m_videoView->setModel(m_videoModel);

    // Set up the tag view
    QGroupBox *tagContainer = new QGroupBox(this);
    tagContainer->setLayout(new QVBoxLayout);
    //tagContainer->setTitle("Tags");
    m_tagFilterEdit = new QLineEdit(this);
    m_tagFilterEdit->setPlaceholderText("Search tags");
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

    // Add the widgets to the window
    addWidget(tagContainer);
    addWidget(videoContainer);
    addWidget(m_infoPanel);

    // Connect signals
    connect(m_tagModel, SIGNAL(itemChanged(QStandardItem*)), SLOT(updateVideoFilter(QStandardItem*)));
    connect(m_collection, SIGNAL(updated()), SLOT(updateTagModel()));
    connect(m_videoView, SIGNAL(activated(QModelIndex)), SLOT(updateInfoPanel(QModelIndex)));
    connect(m_videoView, SIGNAL(clicked(QModelIndex)), SLOT(updateInfoPanel(QModelIndex)));
    connect(m_tagFilterEdit, SIGNAL(textChanged(QString)), m_tagFilterModel, SLOT(setFilterFixedString(QString)));
    connect(m_videoFilterEdit, SIGNAL(textChanged(QString)), m_videoModel, SLOT(setFilterFixedString(QString)));
    connect(m_infoPanel, SIGNAL(editTags()), SLOT(editTags()));
    connect(m_infoPanel, SIGNAL(fetchTags()), SLOT(fetchTags()));
    connect(m_infoPanel, SIGNAL(createCovers()), SLOT(createCovers()));

    m_videoView->resizeColumnToContents(0);
    //m_videoView->resizeColumnToContents(1);

    setStretchFactor(1, 2);

    QTimer::singleShot(0, m_videoFilterEdit, SLOT(setFocus()));
}


void CollectionView::updateTagModel()
{
    m_tagModel->clear();

    QStringList activeTags = m_videoModel->activeTagFilters();
    QList<QString> allTags = Collection::getTags().toList();
    qSort(allTags);
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
}

void CollectionView::updateVideoFilter(QStandardItem *tag)
{
    if (tag->checkState() == Qt::Unchecked) // Deselected tag
        m_videoModel->removeTag(tag->text());
    else if (tag->checkState() == Qt::Checked)
        m_videoModel->addTag(tag->text());

    updateInfoPanel(m_videoView->currentIndex());
    m_videoView->scrollTo(m_videoView->currentIndex());
}

void CollectionView::updateInfoPanel(const QModelIndex &i)
{
    if (!i.isValid()) return;

    QModelIndex index = i;

    if (index.column() != 1)
        index = index.model()->index(index.row(), 1);

    QString videoName = index.model()->data(index).toString();

    m_infoPanel->setInfo(videoName);
}

void CollectionView::editTags()
{
    TagDialog dialog(m_infoPanel->videoName());
    dialog.show();
    dialog.raise();
    dialog.exec();
    updateTagModel();
}

void CollectionView::fetchTags()
{
    TagFetchDialog dialog(m_infoPanel->videoName(), this);
    dialog.show();
    dialog.raise();
    dialog.exec();
    updateTagModel();
}

void CollectionView::createCovers()
{
    CoverMaker dialog(m_infoPanel->videoName(), this);
    dialog.show();
    dialog.raise();
    dialog.exec();
    Collection::scanForCovers(m_infoPanel->videoName());
    m_videoView->repaint();
    m_infoPanel->setInfo(m_infoPanel->videoName());
}
