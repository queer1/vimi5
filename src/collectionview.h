/*
 * Main view in the application
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
