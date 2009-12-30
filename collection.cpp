// Copyright 2009 cwk

#include <QByteArray>
#include <QDebug>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSqlQuery>

#include "collection.h"

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
        QSqlQuery query;
        query.exec("SELECT video.name, group_concat(tag.name, ', ') "
                               "FROM video, tag, videoTag "
                               "WHERE videoTag.videoId = video.rowid AND videoTag.tagId = tag.rowid "
                               "GROUP BY videoId ORDER BY video.name");
        query.seek(item.row());
        QPixmap cover = getCover(query.value(0).toString());
        if (!cover.isNull() && qMax(cover.height(), cover.width()) > Config::maxCoverSize) {
            float factor = 1;
            factor = Config::maxCoverSize / qMax(cover.height(), cover.width());
            return cover.scaled(cover.width() * factor, cover.height() * factor);
        } else
            return cover;
    } else
        return QSqlQueryModel::data(item, role);
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

    scan(QDir(Config::collectionPath));

    reload();
    emit updated();
    emit statusUpdated("Scan finished.");
}


void Collection::scan(QDir dir)
{
    qDebug() << "Scanning directory: " << dir.path();

    if (m_cachedVideoDirectories.contains(dir.path())) // We already know about this video
        return;

    dir.setNameFilters(Config::movieSuffixes);
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
    dir.setNameFilters(Config::movieSuffixes);
    dir.setFilter(QDir::Files);

    return dir.entryList();
}

QPixmap Collection::getCover(const QString &videoName)
{
    if (videoName.isEmpty())
        return QPixmap();

    QDir dir(getPath(videoName));
    dir.setFilter(QDir::Files);

    dir.setNameFilters(QStringList("*front*.jpg"));
    if (dir.entryInfoList().count() > 0) {
        return QPixmap(dir.entryInfoList().first().absoluteFilePath());
    }

    dir.setNameFilters(QStringList("*cover*.jpg"));
    if (dir.entryInfoList().count() > 0) {
        return QPixmap(dir.entryInfoList().first().absoluteFilePath());
    }

    dir.setNameFilters(QStringList("*.jpg"));
    if (dir.entryInfoList().count() > 0) {
        return QPixmap(dir.entryInfoList().first().absoluteFilePath());
    }

    return QPixmap(); // TODO: get some standard thingy to show here.
}
