// Copyright 2009 cwk

#ifndef COLLECTIONVIEW_H
#define COLLECTIONVIEW_H

#include "collection.h"
#include "videofilterproxymodel.h"
#include <QLineEdit>
#include <QListView>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTreeView>
#include <QListWidget>
class Video;
class MainWindow;
class InfoPanel;

class CollectionView : public QSplitter {

    Q_OBJECT

public:
    CollectionView(MainWindow *parent = 0);

    Collection *collection() { return m_collection; }

signals:
    void statusUpdated(const QString &text);

public slots:
    void updateTagModel();

private slots:
    void updateVideoFilter(QStandardItem *tag);
    void updateInfoPanel(const QModelIndex&);
    void editTags();
    void createCovers();
    void selectTag(const QString &tag);

private:

    QListView *m_tagView;
    QTreeView *m_videoView;
    QStandardItemModel *m_tagModel;
    QStandardItemModel *m_favouriteTagModel;
    QSortFilterProxyModel *m_tagFilterModel;
    QLineEdit *m_tagFilterEdit;
    QLineEdit *m_videoFilterEdit;
    VideoFilterProxyModel *m_videoModel;
    Collection *m_collection;
    InfoPanel *m_infoPanel;
    QListView *m_favouriteTags;
};

#endif // COLLECTIONVIEW_H
