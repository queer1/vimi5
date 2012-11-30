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

#include <QAbstractTableModel>
#include <QHash>
#include <QSet>
class QThread;
class QDir;
class CoverLoader;
class Video;

class Collection : public QAbstractTableModel
{

    Q_OBJECT

public:
    Collection(QObject *parent);
    ~Collection();

    static void addTag(const QString &video, const QString &tag);
    static void removeTag(const QString &video, const QString &tag);
    static QStringList getTags(const QString& videoName = QString());
    static QStringList getFiles(const QString& videoName);
    static QString getPath(const QString &videoName);
    static QPixmap getCover(const QString &videoName, int maxSize = Config::maxCoverSize());
    static void replaceTag(const QString &oldTag, const QString &newTag);

    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & = QModelIndex()) const { return m_videoNames.size(); }
    int columnCount(const QModelIndex &) const { return 3; }
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:
    void scanning(bool);
    void updated();
    void statusUpdated(const QString &text);
    void repaintCover(int row, const QModelIndex &parent);

public slots:
    void rescan();
    void writeCache();
    void scanForCovers(const QString &videoName);

private slots:
    void loadCache();

    void coverLoaded(const QString &name);


private:
    void scan(QDir directory);
    void addVideo(Video *video);


    static QHash<QString, Video*> m_videos;
    static QStringList m_videoNames;
    QThread *m_thread;

    QStringList m_cachedVideoDirectories;
    CoverLoader *m_coverLoader;
    static Collection *instance;
};

#endif // COLLECTION_H
