// Copyright 2009 cwk
// Maybe redistributed under GPLv2 or 3

#include <QByteArray>
#include <QDebug>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSqlQuery>

#include "collection.h"

// Static cover cache
QHash<QString, QImage> Collection::m_coverCache;

Collection::Collection()
    : QSqlQueryModel()
{
    initializeDatabase();

    reload();
    setHeaderData(0, Qt::Horizontal, tr("Name"));
    setHeaderData(1, Qt::Horizontal, tr("Tags"));

    connect(this, SIGNAL(updated()), SLOT(reload()));
}

QVariant Collection::data(const QModelIndex &item, int role) const
{
    if (role == Qt::DecorationRole && item.column() == 0) {
        return getCover(item.data(Qt::DisplayRole).toString(), Config::maxCoverSize());
    } else {
        return QSqlQueryModel::data(item, role);
    }
}

void Collection::initializeDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QDir(path).mkpath(path);
    db.setDatabaseName(path + "/sqlite.db");

    if (!db.open()) {
        QMessageBox::critical(0, tr("Cannot open database"),
            tr("Unable to open/create database."), QMessageBox::Cancel);
        return;
    }

    // Ensure our tables exist (FIXME: not very elegant, this)
    QSqlQuery query(db);
    query.exec("CREATE TABLE video (name VARCHAR(20) UNIQUE NOT NULL, path VARCHAR(200) UNIQUE NOT NULL)");
    query.exec("CREATE TABLE tag (name VARCHAR(20) UNIQUE NOT NULL)");
    query.exec("CREATE TABLE videoTag (tagId INT NOT NULL, videoId INT NOT NULL, UNIQUE(tagId, videoId))");

    /* TEST DATA

    query.exec("insert into video values('foo', '/home/w00t/foo')");
    query.exec("insert into video values('bar', '/home/w00t/bar')");

    query.exec("insert into tag values('hawt')");
    query.exec("insert into tag values('not')");
    query.exec("insert into tag values('care')");

    query.exec("insert into videoTag values(1, 2)");
    query.exec("insert into videoTag values(2, 1)");
    query.exec("insert into videoTag values(3, 1)");
    query.exec("insert into videoTag values(3, 2)");*/
}

void Collection::rescan()
{
    qDebug() << "Rescanning collection ...";
    emit statusUpdated("Starting scan...");

    m_cachedVideoDirectories.clear();
    QSqlQuery query;
    query.exec("SELECT path FROM video");
    while (query.next())
        m_cachedVideoDirectories << query.value(0).toString();

    scan(QDir(Config::collectionPath()));

    reload();
    emit updated();
    emit statusUpdated("Scan finished.");
}


void Collection::scan(QDir dir)
{
    qDebug() << "Scanning directory: " << dir.path();

  //  if (m_cachedVideoDirectories.contains(dir.path())) // We already know about this video
  //      return;

    dir.setNameFilters(Config::movieSuffixes());
    dir.setFilter(QDir::Files);
    if (dir.count() > 0) { // Found movie files
        QSqlQuery query;
        query.prepare("INSERT INTO video(name, path) VALUES (:name, :path)");
        query.bindValue(":name", dir.dirName()); // XXX: Stupid assumption, maybe
        query.bindValue(":path", dir.path());
        query.exec();

        if (dir.exists("tags.txt")) { // There is a tag cache here
            QFile file(dir.filePath("tags.txt"));
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                while (!file.atEnd())
                    addTagToDb(dir.dirName(), QString::fromUtf8(file.readLine().simplified().toLower())); // One tag per line
            }
        }
        if (getTags(dir.dirName()).size() == 0) // All videos need at least one tag, a bug, unfortunately
            addTag(dir.dirName(), "video");
        emit statusUpdated("Video added:" + dir.dirName());

    }

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs | QDir::Executable);
                                                                      // QDir::AllDirs ignores name filter


    foreach(QFileInfo subdir, dir.entryInfoList()) {
        scan(QDir(subdir.filePath())); // Recursively scan
    }
}

void Collection::addTagToDb(QString video, QString tag)
{
    if (tag.isEmpty() || getTags(video).contains(tag))
        return;

    qDebug() << "adding tag: " << tag << " to video: " << video;

    QSqlQuery query;
    query.prepare("INSERT INTO tag(name) VALUES(:tag)");
    query.bindValue(":tag", tag);
    if (!query.exec()) qDebug() << "Tag: " << tag << " exists already";

    query.prepare("INSERT INTO videoTag(videoId, tagId) VALUES("
            "(SELECT rowid FROM video WHERE name = :video),"
            "(SELECT rowid FROM tag WHERE name = :tag))");
    query.bindValue(":tag", tag);
    query.bindValue(":video", video);
    if (!query.exec())
        qWarning() << "Unable to add tag '" << tag << "' to video '" << video << "'.";
}

void Collection::addTag(const QString &video, const QString &rawTag)
{
    QString tag = rawTag.simplified().toLower();

    if (tag.isEmpty() || getTags(video).contains(tag))
        return;

    addTagToDb(video, tag);
    writeTagCache(video);
    reload();
}

void Collection::removeTag(const QString &video, const QString &tag)
{
    if (!getTags(video).contains(tag))
        return;

    QSqlQuery query;

    query.prepare("DELETE FROM videoTag "
                  "WHERE videoId = "
                  "(SELECT rowid FOM video WHERE name = :video) "
                  "AND tagId = "
                  "(SELECT rowid FROM tag WHERE name = :tag)");
    query.bindValue(":video", video);
    query.bindValue(":tag", tag);
    query.exec();

    if (getTags(video).size() == 0) // All videos need at least one tag, a bug, unfortunately
        addTag(video, "video");

    writeTagCache(video);
    reload();
}

void Collection::writeTagCache(const QString &video)
{
    QString filename = getPath(video) + "/tags.txt";
    QFile file(filename + ".tmp");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QByteArray data = getTags(video).join("\n").toUtf8() + "\n";
        if (file.write(data) == data.size()) {
            QFile::remove(filename);
            file.rename(filename + ".tmp", filename);
        }
    }
}

void Collection::reload()
{
    setQuery("SELECT video.name, group_concat(tag.name, ', ') " // FIXME: doesn't show videos without any tags
                           "FROM video, tag, videoTag "
                           "WHERE videoTag.videoId = video.rowid AND videoTag.tagId = tag.rowid "
                           "GROUP BY videoId ORDER BY video.name");
}

QStringList Collection::getTags(const QString &videoName)
{
    QStringList tags;

    if (videoName.isEmpty()) {
        QSqlQuery query;
        query.exec("SELECT DISTINCT name FROM tag");
        while (query.next())
            tags.append(query.value(0).toString());

        return tags;
    }

    QSqlQuery query;
    query.prepare("SELECT DISTINCT tag.name FROM video, tag, videoTag "
                  "WHERE videoTag.videoId = video.rowid "
                  "AND videoTag.tagId = tag.rowid "
                  "AND video.name = :videoName");
    query.bindValue(":videoName", videoName);
    query.exec();

    while (query.next())
        tags << query.value(0).toString();

    return tags;
}

QString Collection::getPath(const QString &videoName)
{
    QSqlQuery query;
    query.prepare("SELECT path FROM video WHERE name = :videoName");
    query.bindValue(":videoName", videoName);
    query.exec();
    query.next();
    return query.value(0).toString();
}

QStringList Collection::getFiles(const QString &videoName)
{

    QDir dir(getPath(videoName));
    dir.setNameFilters(Config::movieSuffixes());
    dir.setFilter(QDir::Files);

    return dir.entryList();
}

QPixmap Collection::getCover(const QString &videoName, int maxSize)
{
    if (videoName.isEmpty())
        return QPixmap();

    QDir dir(getPath(videoName));

    // Check if there is a separate folder with covers
    QStringList names;
    names << "*cover*" << "*Cover*";
    dir.setNameFilters(names);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    if (dir.entryList().count() > 0) {
        dir.cd(dir.entryList().first());
    }

    // Checking the disk is slow, scaling is fast.
    QImage cover;
    if (m_coverCache.contains(videoName))
        cover = m_coverCache.value(videoName);
    else {
        // Try to either find *front*.jpg, *cover*.jpg or just any plain *.jpg
        dir.setFilter(QDir::Files);
        if (dir.entryInfoList(QStringList("*front*.jpg")).count() > 0)
            cover = QImage(dir.entryInfoList(QStringList("*front*.jpg")).first().absoluteFilePath());
        else if (dir.entryInfoList(QStringList("*cover*.jpg")).count() > 0)
            cover = QImage(dir.entryInfoList(QStringList("*cover*.jpg")).first().absoluteFilePath());
        else if (dir.entryInfoList(QStringList("*" + videoName + "*.jpg")).count() > 0)
            cover = QImage(dir.entryInfoList(QStringList("*" + videoName + "*.jpg")).first().absoluteFilePath());
        else if (dir.entryInfoList(QStringList("*.jpg")).count() > 0)
            cover = QImage(dir.entryInfoList(QStringList("*.jpg")).first().absoluteFilePath());

        m_coverCache.insert(videoName, cover);
    }
    if (cover.isNull())
        cover = QImage(":/images/defaultcover.png");

    if (!cover.isNull() && cover.height() > maxSize) {
        float factor = 1;
        factor = (float)maxSize / cover.height(), cover.width();
        return QPixmap::fromImage(quickScale(cover, cover.width() * factor, cover.height() * factor));
    } else
        return QPixmap::fromImage(cover);
}

// Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
// for the explanation of the trick, check out:
// http://www.virtualdub.org/blog/pivot/entry.php?id=116
// http://www.compuphase.com/graphic/scale3.htm
#define AVG(a,b)  ( ((((a)^(b)) & 0xfefefefeUL) >> 1) + ((a)&(b)) )

// assume that the source image is ARGB32 formatted
QImage Collection::quickScale(const QImage &s, int width, int height)
{
    QImage source;
    if (source.format() != QImage::Format_ARGB32)
        source = s.convertToFormat(QImage::Format_ARGB32);
    else
        source = s;

    if (width == 0 || height == 0) {
        qDebug() << "asked to resize to 0";
        return QImage();
    }

    QImage dest(width, height, QImage::Format_ARGB32);

    int sw = source.width();
    int sh = source.height();
    int xs = (sw << 8) / width;
    int ys = (sh << 8) / height;
    quint32 *dst = reinterpret_cast<quint32*>(dest.bits());
    int stride = dest.bytesPerLine() >> 2;

    for (int y = 0, yi = ys >> 2; y < height; ++y, yi += ys, dst += stride) {
        const quint32 *src1 = reinterpret_cast<const quint32*>(source.scanLine(yi >> 8));
        const quint32 *src2 = reinterpret_cast<const quint32*>(source.scanLine((yi + ys / 2) >> 8));
        for (int x = 0, xi1 = xs / 4, xi2 = xs * 3 / 4; x < width; ++x, xi1 += xs, xi2 += xs) {
            quint32 pixel1 = AVG(src1[xi1 >> 8], src1[xi2 >> 8]);
            quint32 pixel2 = AVG(src2[xi1 >> 8], src2[xi2 >> 8]);
            dst[x] = AVG(pixel1, pixel2);
        }
    }

    return dest;
}
