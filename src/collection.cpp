// Copyright 2009 cwk
// Maybe redistributed under GPLv2 or 3

#include <QByteArray>
#include <QDebug>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSet>

#include "collection.h"
QHash<QString, Video> Collection::m_videos;
QStringList Collection::m_videoNames;

Collection::Collection()
    : QAbstractTableModel()
{
    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QDir(path).mkpath(path);
    QFile file(path + "/videos.db");
    if (file.open(QIODevice::ReadOnly)) {
        while (!file.atEnd()) {
            Video *video = new Video(QString::fromUtf8(file.readLine()).simplified());
            addVideo(*video);
        }
    } else {
        qWarning() << file.errorString() << path;
    }

    reload();
    setHeaderData(0, Qt::Horizontal, tr("Name"));
    setHeaderData(1, Qt::Horizontal, tr("Tags"));

    connect(this, SIGNAL(updated()), SLOT(reload()));
}

Collection::~Collection()
{
    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QDir(path).mkpath(path);
    QFile file(path + "/videos.db");
    if (file.open(QIODevice::WriteOnly)) {
        foreach(const Video &video, m_videos) {
            file.write(video.path().toUtf8());
        }
        file.close();
    } else {
        qWarning() << file.errorString();
    }
}

QVariant Collection::data(const QModelIndex &item, int role) const
{
    if (!item.isValid() || (role != Qt::DisplayRole && role != Qt::DecorationRole))
        return QVariant();

    if (role == Qt::DecorationRole && item.column() == 0) {
        return m_videos[m_videoNames.at(item.row())].cover(Config::maxCoverSize());
    } else if (role == Qt::DisplayRole && item.column() == 0) {
        return m_videos[m_videoNames.at(item.row())].name();
    } else if (role == Qt::DisplayRole && item.column() == 1) {
        return m_videos[m_videoNames.at(item.row())].tagList();
    } else if (role == Qt::SizeHintRole && item.column() == 0) {
        return m_videos[m_videoNames.at(item.row())].cover(Config::maxCoverSize()).size();
    } else {
        return QVariant();
    }
}

void Collection::addVideo(const Video &video)
{
    if (m_videos.contains(video.name())) {
        qWarning() << video.name();
        int index = m_videoNames.indexOf(video.name());
        beginRemoveRows(QModelIndex(), index, index);
        m_videos.remove(video.name());
        m_videoNames.removeAt(index);
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    m_videos.insert(video.name(), video);
    m_videoNames.append(video.name());
    endInsertRows();

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
    qDebug() << "Rescanning collection ...";
    emit statusUpdated("Starting scan...");

    m_cachedVideoDirectories.clear();
 /*   QSqlQuery query;
    query.exec("SELECT path FROM video");
    while (query.next())
        m_cachedVideoDirectories << query.value(0).toString();*/

    scan(QDir(Config::collectionPath()));

    reload();
    emit updated();
    emit statusUpdated("Scan finished.");

    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QDir(path).mkpath(path);
    QFile file(path + "/videos.db");
    if (file.open(QIODevice::WriteOnly)) {
        foreach(const Video &video, m_videos) {
            file.write(video.path().toUtf8() + "\n");
        }
        file.close();
    } else {
        qWarning() << file.errorString();
    }
}

void Collection::scan(QDir dir)
{
    qDebug() << "Scanning directory: " << dir.path();

  //  if (m_cachedVideoDirectories.contains(dir.path())) // We already know about this video
  //      return;

    dir.setNameFilters(Config::movieSuffixes());
    dir.setFilter(QDir::Files);
    if (dir.count() > 0) { // Found movie files
        addVideo(Video(dir.path()));
    }

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs | QDir::Executable);
                                                                      // QDir::AllDirs ignores name filter


    foreach(QFileInfo subdir, dir.entryInfoList()) {
        scan(QDir(subdir.filePath())); // Recursively scan
    }
}

void Collection::addTag(const QString &video, const QString &rawTag)
{
    QString tag = rawTag.simplified().toLower();

    if (tag.isEmpty())
        return;
    m_videos[video].addTag(tag);
    reload();
}

void Collection::removeTag(const QString &video, const QString &tag)
{
    m_videos[video].removeTag(tag);
    reload();
}

void Collection::reload()
{
    /*m_videoNames.clear();
    foreach(const Video &video, m_videos) {
        m_videoNames.append(video.name());
    }
*/
}

QSet<QString> Collection::getTags(const QString &videoName)
{

    if (videoName.isEmpty()) {
        QSet<QString> allTags;
        foreach(const Video &video, m_videos) {
            allTags.unite(video.tags());
        }
        return allTags;
    } else
        return m_videos[videoName].tags();
}

QString Collection::getPath(const QString &videoName)
{
    return m_videos[videoName].path();
}

QStringList Collection::getFiles(const QString &videoName)
{
    return m_videos[videoName].files();
}

QPixmap Collection::getCover(const QString &videoName, int maxSize)
{
    return m_videos[videoName].cover(maxSize);
}
