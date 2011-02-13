// Copyright 2009 cwk

#ifndef COLLECTIONVIEW_H
#define COLLECTIONVIEW_H

#include "collection.h"
#include "infopanel.h"
#include "videofilterproxymodel.h"
#include <QLineEdit>
#include <QListView>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTreeView>

class CollectionView : public QSplitter {

    Q_OBJECT

public:
    CollectionView(QWidget *parent = 0);

    Collection *collection() { return m_collection; }

private slots:
    void updateVideoFilter(QStandardItem *tag);
    void updateTagModel();
    void updateInfoPanel(const QModelIndex&);
    void editTags();
    void fetchTags();

private:
    QListView *m_tagView;
    QTreeView *m_videoView;
    QStandardItemModel *m_tagModel;
    QSortFilterProxyModel *m_tagFilterModel;
    QLineEdit *m_tagFilterEdit;
    QLineEdit *m_videoFilterEdit;
    VideoFilterProxyModel *m_videoModel;
    Collection *m_collection;
    InfoPanel *m_infoPanel;
};

#endif // COLLECTIONVIEW_H
