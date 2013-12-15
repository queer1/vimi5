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
#include <QGuiApplication>

#include <ctime>
#include <algorithm>

#include "collection.h"
#include "videoframedumper.h"

QDataStream &operator<<(QDataStream &datastream, const Video &video) {
    datastream << video.m_name << video.m_path << video.m_cover << video.m_files << video.m_tags << video.m_lastPosition << video.m_lastFile << video.m_bookmarks << video.m_screenshots;
    return datastream;
}

QDataStream &operator>>(QDataStream &datastream, Video &video) {
    datastream >> video.m_name >> video.m_path >> video.m_cover >> video.m_files >> video.m_tags >> video.m_lastPosition >> video.m_lastFile >> video.m_bookmarks >> video.m_screenshots;
    return datastream;
}


bool compareVideos(const Video *a, const Video *b)
{
    return a->m_name < b->m_name;
}

Collection::Collection()
    : QAbstractListModel()
{
    loadCache();

    // Check if cache load was successful
    if (m_videos.count() == 0)
        rescan();

    updateActresses();
}

void Collection::updateActresses()
{
    QDir dir(QUrl(Config::instance()->actressesPath()).toLocalFile());
    QFileInfoList fileList = dir.entryInfoList(QStringList() << "*.jpg" << "*.png", QDir::Files, QDir::Name);
    QStringList tags = allTags();
    QStringList newActressList;
    foreach (const QFileInfo &file, fileList) {
        if (tags.contains(file.baseName())) {
            newActressList.append(file.baseName());
        }
    }
    if (newActressList != m_actresses) {
        m_actresses = newActressList;
        emit actressesChanged();
    }
}


QHash<int, QByteArray> Collection::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[Video::NameRole] = "name";
    roleNames[Video::PathRole] = "path";
    roleNames[Video::CoverRole] = "coverPath";
    roleNames[Video::FilesRole] = "files";
    roleNames[Video::TagsRole] = "tags";
    roleNames[Video::LastPositionRole] = "lastPosition";
    roleNames[Video::LastFileRole] = "lastFile";
    roleNames[Video::BookmarksRole] = "bookmarks";
    roleNames[Video::ScreenshotsRole] = "screenshots";
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
        out << m_videos;
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
        m_videos.clear();
        in >> m_videos;
        updateFilteredVideos();
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

    if (row > m_videos.count()) {
        qWarning() << "row out of bounds" << row;
        return QVariant();
    }

    switch(role){
    case Video::NameRole:
        return m_filteredVideos[row]->m_name;
    case Video::PathRole:
        return m_filteredVideos[row]->m_path;
    case Video::CoverRole:
        return m_filteredVideos[row]->m_cover;
    case Video::FilesRole:
        return m_filteredVideos[row]->m_files;
    case Video::TagsRole:
        return m_filteredVideos[row]->m_tags;
    case Video::LastPositionRole:
        return m_filteredVideos[row]->m_lastPosition;
    case Video::LastFileRole:
        return m_filteredVideos[row]->m_lastFile;
    case Video::BookmarksRole:
        return m_filteredVideos[row]->m_bookmarks;
    case Video::ScreenshotsRole:
        return m_filteredVideos[row]->m_screenshots;
    default:
        qWarning() << "Unknown role" << role;
        return QVariant();
    }
}

void Collection::writeBookmarkCache(int index)
{
    QString filename = m_filteredVideos[index]->m_path + "/bookmarks.dat";
    QFile file(filename + ".tmp");
    QDataStream out(&file);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        out << m_filteredVideos[index]->m_bookmarks;
        QFile::remove(filename);
        file.rename(filename + ".tmp", filename);
    } else {
        qWarning() << "Unable to open tag cache file for writing!:" << filename;
    }
}


void Collection::addBookmark(int row, QString file, int bookmark)
{
    QList<QVariant> bookmarks = m_filteredVideos[row]->m_bookmarks[file].toList();
    bookmarks.append(bookmark);
    m_filteredVideos[row]->m_bookmarks[file] = bookmarks;

    emit dataChanged(createIndex(row, 0), createIndex(row, 1));
    writeBookmarkCache(row);
}

void Collection::removeBookmark(int row, QString file, int bookmark)
{
    if (!m_filteredVideos[row]->m_bookmarks.contains(file))
        return;

    QList<QVariant> bookmarks = m_filteredVideos[row]->m_bookmarks[file].toList();
    bookmarks.removeAll(bookmark);
    m_filteredVideos[row]->m_bookmarks[file] = bookmarks;

    emit dataChanged(createIndex(row, 0), createIndex(row, 0));
    writeBookmarkCache(row);
}

void Collection::writeTagCache(int index)
{
    QString filename = m_filteredVideos[index]->m_path + "/tags.txt";
    QFile file(filename + ".tmp");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QByteArray data = m_filteredVideos[index]->m_tags.join("\n").toUtf8() + "\n";
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
    if (m_filteredVideos[row]->m_tags.contains(tag))
        return;
    if (tag.isEmpty())
        return;

    m_filteredVideos[row]->m_tags.append(tag);
    emit dataChanged(createIndex(row, 0), createIndex(row, 1));
    emit tagsUpdated();
    writeTagCache(row);
}

void Collection::removeTag(int row, QString tag)
{
    m_filteredVideos[row]->m_tags.removeAll(tag);
    emit dataChanged(createIndex(row, 0), createIndex(row, 0));
    emit tagsUpdated();
    writeTagCache(row);
}


void Collection::setLastFile(int row, QString file)
{
    m_filteredVideos[row]->m_lastFile = file;
    emit dataChanged(createIndex(row, 0), createIndex(row, 0));
}


void Collection::setLastPosition(int row, int position)
{
    m_filteredVideos[row]->m_lastPosition = position;
    emit dataChanged(createIndex(row, 0), createIndex(row, 0));
}


void Collection::rescan()
{
    qDebug() << "Starting scan...";
    setStatus("Starting scan...");

    beginResetModel();
    m_filteredVideos.clear();
    m_videos.clear();
    endResetModel();
    scan(QDir(Config::instance()->collectionPath()));
    qDebug() << QDir(Config::instance()->collectionPath());
    //scan(QDir("/home/test/Nubile films ultimate creampie"));
    qSort(m_filteredVideos.begin(), m_filteredVideos.end(), compareVideos);


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
    if (dir.exists(".vimicover.jpg")) {
        coverPath = dir.absoluteFilePath(".vimicover.jpg");
    } else if (dir.entryInfoList(QStringList("*front*.jpg")).count() > 0) {
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
    qDebug() << "Scanning directory: " << dir.path();
    //QGuiApplication::instance()->processEvents();

    dir.setNameFilters(Config::instance()->movieSuffixes());
    dir.setFilter(QDir::Files);

    ////////////////////
    // Found movie files
    if (dir.count() > 0) {
        //qDebug() << "Found video " << dir.dirName();

        QString name = dir.dirName();
        QString path = dir.path();
        QString cover = scanForCovers(path);
        QStringList files = dir.entryList();
        QStringList tags;
        QVariantMap bookmarks;
        QStringList screenshots;

        // Load tag cache
        if (dir.exists("tags.txt")) {
            QFile file(dir.filePath("tags.txt"));
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                while (!file.atEnd()) {
                    QString tag = QString::fromUtf8(file.readLine().simplified().toLower());
                    if (!tag.isEmpty())
                        tags.append(tag); // One tag per line
                }
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

        // Look for screenshots
        QMap <qint64, QString> fileMap;
        foreach(const QString &file, dir.entryList(QStringList() << ".vimiframe_*_*.jpg", QDir::Files | QDir::Hidden)) {
    //        qDebug() << file;
            fileMap[file.split("_")[1].toLong()] = file;
        }
        screenshots = fileMap.values();

        m_videos.append(Video(name, path, cover, files, tags, 0, files.first(), bookmarks, screenshots));

        int i = 0;
        foreach(Video *video, m_filteredVideos) {
            if (video->m_name > m_videos.last().m_name) {
                beginInsertRows(QModelIndex(), i, i);
                m_filteredVideos.insert(i, &m_videos.last());
                endInsertRows();
                break;
            }
            i++;
        }
        if (i >= m_filteredVideos.size()) {
            beginInsertRows(QModelIndex(), i, i);
            m_filteredVideos.append(&m_videos.last());
            endInsertRows();
        }

        setStatus(tr("Found video ")  + name);
    }

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs | QDir::Executable);
    // QDir::AllDirs ignores name filter
    foreach(const QFileInfo &subdir, dir.entryInfoList()) {
        scan(QDir(subdir.filePath())); // Recursively scan
    }
}


bool videoContainsTags(const Video &video, const QStringList &tags)
{
    foreach (const QString &tag, tags)
        if (!video.m_tags.contains(tag)) return false;

    return true;
}

QStringList Collection::allTags()
{
    QMap<QString, int> allTags;
    foreach(const Video &video, m_videos) {
        if (!videoContainsTags(video, m_filterTags))
            continue;

        foreach(const QString &tag, video.m_tags) {
            if (!m_tagFilter.isEmpty() && !tag.contains(m_tagFilter)) continue;

            if (!allTags.contains(tag)) allTags[tag] = 0;
            else allTags[tag]++;
        }
    }
    QMultiMap<int, QString> reverse;
    foreach(const QString &key, allTags.keys()) {
        //if (Config::instance()->actresses().contains(key)) continue;
        reverse.insert(allTags[key], key);
    }

    QStringList ret = reverse.values();
    std::reverse(ret.begin(), ret.end());
    return ret;
}


void Collection::updateFilteredVideos()
{
    clock_t start, end;
    start = clock();

    QList<Video*> filtered;
    for (int i=0; i<m_videos.count(); i++) {
        if (!m_filter.isEmpty() && !m_videos[i].m_name.contains(m_filter))
            continue;

        if (m_filterTags.isEmpty() || videoContainsTags(m_videos[i], m_filterTags)) {
            filtered.append(&m_videos[i]);
        }
    }
    if (filtered == m_filteredVideos) {
        return;
    }

    qSort(filtered.begin(), filtered.end(), compareVideos);

    //TODO intelligently merge and notify about updates
    beginResetModel();
    m_filteredVideos = filtered;
    endResetModel();
    end = clock();
    qDebug() << "update finished in" << ((float) (end - start) / CLOCKS_PER_SEC) << "seconds.";
}

void Collection::setFilter(QString text)
{
    m_filter = text;
    updateFilteredVideos();
}


void Collection::addFilterTag(QString tag)
{
    m_filterTags.append(tag);
    for (int i=m_filteredVideos.count()-1; i>=0; --i) {
        if (!m_filter.isEmpty() && !m_filteredVideos[i]->m_name.contains(m_filter))
            continue;

        if (!m_filteredVideos[i]->m_tags.contains(tag)) {
            beginRemoveRows(QModelIndex(), i, i);
            m_filteredVideos.removeAt(i);
            endRemoveRows();
        }
    }

    emit tagsUpdated();
    updateActresses();
}


void Collection::removeFilterTag(QString tag)
{
    m_filterTags.removeAll(tag);

    for (int i=m_videos.count()-1; i>=0; --i) {
        if (!m_filter.isEmpty() && !m_videos[i].m_name.contains(m_filter))
            continue;

        if (m_filteredVideos.contains(&m_videos[i]))
            continue;

        if (videoContainsTags(m_videos[i], m_filterTags)) {
            int idx=0;
            foreach(Video *video, m_filteredVideos) {
                if (video->m_name > m_videos[i].m_name)
                    break;
                else idx++;
            }

            beginInsertRows(QModelIndex(), idx, idx);
            m_filteredVideos.insert(idx, &m_videos[i]);
            endInsertRows();
        }
    }


    emit tagsUpdated();
    updateActresses();
}

void Collection::createCover(QString file, qint64 position)
{
    VideoFrameDumper *dumper = new VideoFrameDumper(file);
    qDebug() << "creating cover" << position;
    connect(dumper, SIGNAL(coverCreated(QString)), SLOT(coverCreated(QString)));
    connect(dumper, SIGNAL(statusUpdated(QString)), SLOT(setStatus(QString)));
    //dumper->createCover(position);
}

void Collection::createScreenshots(QUrl file)
{
    VideoFrameDumper *dumper = new VideoFrameDumper(file);
    connect(dumper, SIGNAL(screenshotsCreated(QString)), SLOT(screenshotsCreated(QString)));
    connect(dumper, SIGNAL(statusUpdated(QString)), SLOT(setStatus(QString)));
    qDebug() << QMetaObject::invokeMethod(dumper, "createSnapshots");
}

void Collection::screenshotsCreated(QString path)
{
    qDebug() << path;
    for (int row=0; row<m_filteredVideos.count(); row++) {
        if (m_filteredVideos[row]->m_path == path) {
            QDir dir(path);
            QMap <qint64, QString> fileMap;
            foreach(const QString &file, dir.entryList(QStringList() << ".vimiframe_*_*.jpg", QDir::Files | QDir::Hidden)) {
                fileMap[file.split("_")[1].toLong()] = file;
            }
            m_filteredVideos[row]->m_screenshots = fileMap.values();
            //qDebug() << fileMap;
            emit dataChanged(index(row), index(row));
            return;
        }
    }
}

void Collection::coverCreated(QString path)
{
    for (int row=0; row<m_filteredVideos.count(); row++) {
        if (m_filteredVideos[row]->m_path == path) {
            m_filteredVideos[row]->m_cover = scanForCovers(path);
            emit dataChanged(createIndex(row, 0), createIndex(row, 1));
            qDebug() << row;
            return;
        }
    }
}
