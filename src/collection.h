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

class Video
{
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        CoverRole,
        FilesRole,
        TagsRole,
        LastPositionRole,
        LastFileRole,
        BookmarksRole
    };

    Video(QString name, QString path, QString cover, QStringList files, QStringList tags, int lastPosition, QString lastFile, QVariantMap bookmarks) :
        m_name(name), m_path(path), m_cover(cover), m_files(files), m_tags(tags), m_lastPosition(lastPosition), m_lastFile(lastFile), m_bookmarks(bookmarks)
    { }

    Video() {}

    friend class Collection;
    friend QDataStream &operator<<(QDataStream &, const Video &);
    friend QDataStream &operator>>(QDataStream &, Video &);

    QString m_name;
    QString m_path;
    QString m_cover;
    QStringList m_files;
    QStringList m_tags;
    int m_lastPosition;
    QString m_lastFile;
    QVariantMap m_bookmarks;
};

class Collection : public QAbstractListModel
{

    Q_OBJECT
    Q_PROPERTY(QStringList allTags READ allTags NOTIFY tagsUpdated)

public:

    Collection();
    ~Collection();

    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & = QModelIndex()) const { return m_filteredVideos.size(); }
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

    void setFilter(QString text);
    void addFilterTag(QString tag);
    void removeFilterTag(QString tag);

    void setTagFilter(QString text) { m_tagFilter = text; emit tagsUpdated(); }

    QStringList allTags();


signals:
    void tagsUpdated();

private:
    void scan(QDir directory);
    void loadCache();
    void writeTagCache(int index);
    void writeBookmarkCache(int index);
    void updateFilteredVideos();

    QList <Video> m_videos;
    QList <Video*> m_filteredVideos;
    QString m_filter;
    QString m_tagFilter;
    QStringList m_filterTags;
//    QStringList m_filteredNames;
//    QStringList m_filteredPaths;
//    QStringList m_filteredCovers;
//    QList<QStringList> m_filteredFiles;
//    QList<QStringList> m_filteredTags;
//    QList<int> m_filteredLastPositions;
//    QStringList m_filteredLastFile;
//    QList<QVariantMap> m_filteredBookmarks;
};

#endif // COLLECTION_H

