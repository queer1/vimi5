// Copyright 2009 cwk
// Maybe redistributed under GPLv2 or 3

#include <QByteArray>
#include <QDebug>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSet>
#include <QThread>
#include <QApplication>
#include <QDir>
#include "video.h"
#include "coverloader.h"

#include "collection.h"
QHash<QString, Video*> Collection::m_videos;
QStringList Collection::m_videoNames;
Collection *Collection::instance = 0;

Collection::Collection(QObject *parent)
    : QAbstractTableModel(parent)
{
    instance = this;
    m_coverLoader = new CoverLoader;
    QThread *thread = new QThread(this);
    m_coverLoader->moveToThread(thread);
    thread->start();

    loadCache();
}

void Collection::loadCache()
{
    qDebug() << "Loading cache...";
    emit(statusUpdated("Loading video cache..."));
    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QDir(path).mkpath(path);
    QFile file(path + "/videos.db");
    QDataStream in(&file);
    if (file.open(QIODevice::ReadOnly)) {
        QVector<Video*> videos;
        while (!file.atEnd()) {
            QString path, tags, coverPath;
            in >> path;
            in >> tags;
            in >> coverPath;
            Video *video = new Video(this, path, tags, coverPath);
            videos.append(video);
            connect(video, SIGNAL(needToLoadCover(Video*)), m_coverLoader, SLOT(loadVideo(Video*)));
            connect(video, SIGNAL(coverLoaded(QString)), this, SLOT(coverLoaded(QString)));
        }
        beginInsertRows(QModelIndex(), rowCount(), rowCount() + videos.size() - 1);
        foreach(Video *video, videos) {
            if (m_videos.contains(video->name())) {
                qWarning() << video->name() << "already in collection (" << m_videos[video->name()]->path() << " vs the new:" << video->path() << ").";
            }
            m_videos.insert(video->name(), video);
            m_videoNames.append(video->name());
        }
        endInsertRows();
        qDebug() << "Loaded" << videos.size() << "videos.";
    } else {
        qWarning() << "Unable to load cache!: " << file.errorString() << " (file path:" << path << ")";
    }
    qSort(m_videoNames);
    emit statusUpdated("Ready!");
}

Collection::~Collection()
{
    m_coverLoader->running = false;
    m_coverLoader->thread()->quit();
    m_coverLoader->thread()->wait();
}

QVariant Collection::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    if (section == 1)
        return tr("Name");
    else if (section == 2)
        return tr("Tags");
    else
        return tr("Cover");
}

QVariant Collection::data(const QModelIndex &item, int role) const
{
    if (!item.isValid() || (role != Qt::DisplayRole && role != Qt::DecorationRole))
        return QVariant();

    if (role == Qt::DecorationRole && item.column() == 0) {
        return m_videos[m_videoNames.at(item.row())]->thumbnail();
    } else if (role == Qt::DisplayRole && item.column() == 1) {
        return m_videos[m_videoNames.at(item.row())]->name();
    } else if (role == Qt::DisplayRole && item.column() == 2) {
        return m_videos[m_videoNames.at(item.row())]->tagList();
    } else if (role == Qt::DisplayRole && item.column() == 3) {
        return m_videos[m_videoNames.at(item.row())]->tags();
    } else if (role == Qt::SizeHintRole) {// && item.column() == 0) {
        //return m_videos[m_videoNames.at(item.row())]->cover(Config::maxCoverSize()).size();
        return QSize(128, 128);
    } else {
        return QVariant();
    }
}

void Collection::addVideo(Video *video)
{
    if (m_videos.contains(video->name())) {
        qWarning() << video->name() << "already in collection (" << m_videos[video->name()]->path() << " vs the new:" << video->path() << ").";
        int index = m_videoNames.indexOf(video->name());
        beginRemoveRows(QModelIndex(), index, index);
        m_videos.remove(video->name());
        m_videoNames.removeAt(index);
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_videos.insert(video->name(), video);
    m_videoNames.append(video->name());
    endInsertRows();

    connect(video, SIGNAL(needToLoadCover(Video*)), m_coverLoader, SLOT(loadVideo(Video*)));
    connect(video, SIGNAL(coverLoaded(QString)), this, SLOT(coverLoaded(QString)));
}

QModelIndex Collection::index(int row, int column, const QModelIndex &) const
{
    return createIndex(row, column, row);
}

bool Collection::hasChildren(const QModelIndex &index) const
{
    return (!index.isValid()); //WTF
}

void Collection::rescan()
{
    emit statusUpdated("Starting scan...");
    emit scanning(true);
    m_cachedVideoDirectories.clear();

    beginRemoveRows(QModelIndex(), 0, rowCount());
    m_videoNames.clear();
    m_videos.clear();
    endRemoveRows();
    scan(QDir(Config::collectionPath()));
    qSort(m_videoNames);

    writeCache();
    emit scanning(false);
    emit updated();
    emit statusUpdated("Scan finished.");
}

void Collection::writeCache()
{
    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QDir(path).mkpath(path);
    QFile file(path + "/videos.db");
    QDataStream out(&file);
    if (file.open(QIODevice::WriteOnly)) {
        emit statusUpdated("writing cache...");
        foreach(Video *video, m_videos) {
            out << video->path() << video->tags().join(",") << video->coverPath();
        }
        file.close();
        emit statusUpdated("Finished writing cache!");
    } else {
        qWarning() << file.errorString();
    }
}

void Collection::scan(QDir dir)
{
    //qDebug() << "Scanning directory: " << dir.path();

    dir.setNameFilters(Config::movieSuffixes());
    dir.setFilter(QDir::Files);
    if (dir.count() > 0) { // Found movie files
        emit statusUpdated("Found video " + dir.dirName());
        addVideo(Video::makeVideo(this, dir.path()));
    }

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs | QDir::Executable);
                                                                      // QDir::AllDirs ignores name filter


    foreach(const QFileInfo &subdir, dir.entryInfoList()) {
        scan(QDir(subdir.filePath())); // Recursively scan
    }
}

void Collection::addTag(const QString &video, const QString &rawTag)
{
    QString tag = rawTag.simplified().toLower();

    if (tag.isEmpty())
        return;
    m_videos[video]->addTag(tag);
    int row = m_videoNames.indexOf(video);
    emit instance->dataChanged(instance->createIndex(row, 3), instance->createIndex(row, 3));
}

void Collection::removeTag(const QString &video, const QString &tag)
{
    m_videos[video]->removeTag(tag);
    int row = m_videoNames.indexOf(video);
    emit instance->dataChanged(instance->createIndex(row, 3), instance->createIndex(row, 3));
}

QStringList Collection::getTags(const QString &videoName)
{
    if (videoName.isEmpty()) {
        QSet<QString> allTags;
        foreach(Video *video, m_videos) {
            allTags.unite(video->tags().toSet());
        }
        QStringList ret = allTags.toList();
        qSort(ret);
        return ret;
    } else
        return m_videos[videoName]->tags();
}

QString Collection::getPath(const QString &videoName)
{
    //if (!m_videos.contains(videoName)) return QString();

    return m_videos[videoName]->path();
}

QStringList Collection::getFiles(const QString &videoName)
{
    //if (!m_videos.contains(videoName)) return QStringList();

    return m_videos[videoName]->files();
}

QPixmap Collection::getCover(const QString &videoName, int maxSize)
{
    //if (!m_videos.contains(videoName)) return QPixmap();

    return m_videos[videoName]->cover(maxSize);
}

void Collection::scanForCovers(const QString &videoName)
{
    m_videos[videoName]->rescanForCovers();
    m_videos[videoName]->generateThumbnail();
}

void Collection::coverLoaded(const QString &videoName)
{
    int row = m_videoNames.indexOf(videoName);
    //emit repaintCover(row, createIndex(row, 0, row));
    emit dataChanged(createIndex(row, 0), createIndex(row, 0));
}

void Collection::replaceTag(const QString &oldTag, const QString &newTag)
{
    foreach(Video *video, m_videos) {
        if (video->tags().contains(oldTag)) {
            qDebug() << "replacing tags for video: " << video->name() << oldTag << newTag;
            removeTag(video->name(), oldTag);
            addTag(video->name(), newTag);
        }
    }
}
