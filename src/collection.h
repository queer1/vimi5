/*
 * Main collection model, holds list of all videos and tags
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

#ifndef COLLECTION_H
#define COLLECTION_H

#include "config.h"

#include <QAbstractListModel>
#include <QHash>
#include <QDebug>

class QThread;
class QDir;
class Video;

class Collection : public QAbstractListModel
{

    Q_OBJECT
    Q_PROPERTY(QStringList allTags READ allTags NOTIFY tagsUpdated)

public:
    enum VideoRoles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        CoverRole,
        FilesRole,
        TagsRole,
        LastPositionRole,
        LastFileRole,
        BookmarksRole
    };

    Collection();
    ~Collection();

    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & = QModelIndex()) const { return m_names.size(); }
    QHash<int, QByteArray> roleNames() const;

public slots:
    void rescan();
    void writeCache();

    void addTag(int row, QString tag);
    void removeTag(int row, QString tag);

    void addBookmark(int row, QString file, int bookmark );
    void removeBookmark(int row, QString file, int bookmark);


    void setLastFile(int row, QString file);
    void setLastPosition(int row, int position);

    QStringList allTags();


signals:
    void tagsUpdated();

private:
    void scan(QDir directory);
    void loadCache();
    void writeTagCache(int index);
    void writeBookmarkCache(int index);

    QStringList m_names;
    QStringList m_paths;
    QStringList m_covers;
    QList<QStringList> m_files;
    QList<QStringList> m_tags;
    QList<int> m_lastPositions;
    QStringList m_lastFile;
    //QList<QMap<QString, QList<int > > > m_bookmarks;
    QList<QVariantMap> m_bookmarks;
};

#endif // COLLECTION_H
