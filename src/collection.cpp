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

#include <QByteArray>
#include <QDebug>
#include <QStandardPaths>
#include <QSet>
#include <QThread>
#include <QDir>

#include "collection.h"

Collection::Collection()
    : QAbstractListModel()
{
    loadCache();
    if (m_names.isEmpty())
        rescan();
}


QHash<int, QByteArray> Collection::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[NameRole] = "name";
    roleNames[PathRole] = "path";
    roleNames[CoverRole] = "coverPath";
    roleNames[FilesRole] = "files";
    roleNames[TagsRole] = "tags";
    roleNames[LastPositionRole] = "lastPosition";
    roleNames[LastFileRole] = "lastFile";
    roleNames[BookmarksRole] = "bookmarks";
    return roleNames;
}

Collection::~Collection()
{
    writeCache();
}

void Collection::writeCache()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir(path).mkpath(path);
    QFile file(path + "/videos.db");
    QDataStream out(&file);

    if (file.open(QIODevice::WriteOnly)) {
        out << m_names << m_paths << m_covers << m_files << m_tags << m_lastPositions << m_lastFile << m_bookmarks;
    } else {
        qWarning() << file.errorString();
    }
}

void Collection::loadCache()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir(path).mkpath(path);
    QFile file(path + "/videos.db");
    QDataStream in(&file);

    if (file.open(QIODevice::ReadOnly)) {
        beginResetModel();
        in >> m_names;
        in >> m_paths;
        in >> m_covers;
        in >> m_files;
        in >> m_tags;
        in >> m_lastPositions;
        in >> m_lastFile;
        in >> m_bookmarks;
        endResetModel();
    } else {
        qWarning() << "Unable to load cache!: " << file.errorString() << " (file path:" << path << ")";
    }
}

QVariant Collection::data(const QModelIndex &item, int role) const
{
    if (!item.isValid()) {
        qWarning() << "Invalid item!";
        return QVariant();
    }

    int row = item.row();

    if (row > m_names.count()) {
        qWarning() << "row out of bounds" << row;
        return QVariant();
    }

    switch(role){
    case NameRole:
        return m_names[row];
    case PathRole:
        return m_paths[row];
    case CoverRole:
        return m_covers[row];
    case FilesRole:
        return m_files[row];
    case TagsRole:
        return m_tags[row];
    case LastPositionRole:
        return m_lastPositions[row];
    case LastFileRole:
        return m_lastFile[row];
    case BookmarksRole: {
     /*   const QMap<QString, QList<int > > &bookmarks = m_bookmarks[row];
        QVariantMap map;
        foreach(const QString &key, bookmarks.keys()) {
            QVariantList list;
            foreach(int bookmark, bookmarks[key])
                list.append(bookmark);
            map[key] = list;
        }

        return map;*/
        return m_bookmarks[row];
    }
    default:
        qWarning() << "Unknown role" << role;
        return QVariant();
    }
}

void Collection::writeBookmarkCache(int index)
{
    QString filename = m_paths[index] + "/bookmarks.dat";
    QFile file(filename + ".tmp");
    QDataStream out(&file);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        out << m_bookmarks[index];
        QFile::remove(filename);
        file.rename(filename + ".tmp", filename);
    } else {
        qWarning() << "Unable to open tag cache file for writing!:" << filename;
    }
}


void Collection::addBookmark(int row, QString file, int bookmark)
{
    QList<QVariant> bookmarks = m_bookmarks[row][file].toList();
    bookmarks.append(bookmark);
    m_bookmarks[row][file] = bookmarks;

    emit dataChanged(createIndex(row, 0), createIndex(row, 1));
    writeBookmarkCache(row);
}

void Collection::removeBookmark(int row, QString file, int bookmark)
{
    qDebug() << row << file << bookmark << m_bookmarks[row];
    if (!m_bookmarks[row].contains(file))
        return;

    QList<QVariant> bookmarks = m_bookmarks[row][file].toList();
    bookmarks.removeAll(bookmark);
    m_bookmarks[row][file] = bookmarks;

    emit dataChanged(createIndex(row, 0), createIndex(row, 0));
    writeBookmarkCache(row);
}

void Collection::writeTagCache(int index)
{
    QString filename = m_paths[index] + "/tags.txt";
    QFile file(filename + ".tmp");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QByteArray data = m_tags[index].join("\n").toUtf8() + "\n";
        if (file.write(data) == data.size()) {
            QFile::remove(filename);
            file.rename(filename + ".tmp", filename);
        }
    } else {
        qWarning() << "Unable to open tag cache file for writing!:" << filename;
    }
}

void Collection::addTag(int row, QString tag)
{
    if (m_tags[row].contains(tag))
        return;

    m_tags[row].append(tag);
    emit dataChanged(createIndex(row, 0), createIndex(row, 1));
    emit tagsUpdated();
    writeTagCache(row);
}

void Collection::removeTag(int row, QString tag)
{
    m_tags[row].removeAll(tag);
    emit dataChanged(createIndex(row, 0), createIndex(row, 0));
    emit tagsUpdated();
    writeTagCache(row);
}


void Collection::setLastFile(int row, QString file)
{
    m_lastFile[row] = file;
    emit dataChanged(createIndex(row, 0), createIndex(row, 0));
}


void Collection::setLastPosition(int row, int position)
{
    m_lastPositions[row] = position;
    emit dataChanged(createIndex(row, 0), createIndex(row, 0));
}


void Collection::rescan()
{
    qDebug() << "Starting scan...";

    beginResetModel();
    m_names.clear();
    m_covers.clear();
    m_files.clear();
    m_lastFile.clear();
    m_lastPositions.clear();
    m_tags.clear();
    m_paths.clear();
    scan(QDir(Config::collectionPath()));
    endResetModel();

    writeCache();
}

static QString scanForCovers(QString path)
{
    QDir dir(path);

    // Check if there is a separate folder with covers
    QStringList names;
    names << "*cover*" << "*Cover*";
    dir.setNameFilters(names);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    if (dir.entryList().count() > 0) {
        dir.cd(dir.entryList().first());
    }

    QString coverPath;
    // Try to either find *front*.jpg, *cover*.jpg or just any plain *.jpg
    dir.setFilter(QDir::Files);
    if (dir.entryInfoList(QStringList("*front*.jpg")).count() > 0) {
        coverPath = dir.entryInfoList(QStringList("*front*.jpg")).first().absoluteFilePath();
    } else if (dir.entryInfoList(QStringList("*cover*.jpg")).count() > 0) {
        coverPath = dir.entryInfoList(QStringList("*cover*.jpg")).first().absoluteFilePath();
    } else if (dir.entryInfoList(QStringList(dir.dirName() + ".jpg")).count() > 0) {
        coverPath = dir.entryInfoList(QStringList(dir.dirName() + ".jpg")).first().absoluteFilePath();
    } else if (dir.entryInfoList(QStringList("*" + dir.dirName() + "*.jpg")).count() > 0) {
        coverPath = dir.entryInfoList(QStringList("*" + dir.dirName() + "*.jpg")).first().absoluteFilePath();
    } else if (dir.entryInfoList(QStringList("*.jpg")).count() > 0) {
        coverPath = dir.entryInfoList(QStringList("*.jpg")).first().absoluteFilePath();
    }

    return coverPath;
}

void Collection::scan(QDir dir)
{
    //qDebug() << "Scanning directory: " << dir.path();

    dir.setNameFilters(Config::movieSuffixes());
    dir.setFilter(QDir::Files);

    ////////////////////
    // Found movie files
    if (dir.count() > 0) {
        qDebug() << "Found video " << dir.dirName();

        QString name = dir.dirName();
        QString path = dir.path();
        QString cover = scanForCovers(path);
        QStringList files = dir.entryList();
        QStringList tags;
        QVariantMap bookmarks;

        // Load tag cache
        if (dir.exists("tags.txt")) {
            QFile file(dir.filePath("tags.txt"));
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                while (!file.atEnd())
                    tags.append(QString::fromUtf8(file.readLine().simplified().toLower())); // One tag per line
            }
        }

        // Load tag cache
        if (dir.exists("bookmarks.dat")) {
            QFile file(dir.filePath("bookmarks.dat"));
            QDataStream in(&file);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                in >> bookmarks;
            }
        }



        m_names.append(name);
        m_paths.append(path);
        m_covers.append(cover);
        m_files.append(files);
        m_tags.append(tags);
        m_lastPositions.append(0);
        m_lastFile.append(m_files.last().first());
        m_bookmarks.append(bookmarks);
    }

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs | QDir::Executable);
    // QDir::AllDirs ignores name filter
    foreach(const QFileInfo &subdir, dir.entryInfoList()) {
        scan(QDir(subdir.filePath())); // Recursively scan
    }
}

QStringList Collection::allTags()
{
    qDebug() << "slow";
    QSet<QString> allTags;
    foreach(const QStringList &taglist, m_tags)
        foreach(const QString &tag, taglist)
            if (!allTags.contains(tag))
                allTags.insert(tag);

    QStringList ret = allTags.toList();
    qSort(ret);
    return ret;

}
