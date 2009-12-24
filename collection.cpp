// Copyright 2009 cwk

#include <QDebug>
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

void Collection::initializeDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("vimi.db");
    if (!db.open()) {
        QMessageBox::critical(0, tr("Cannot open database"),
            tr("Unable to establish a database connection.\n"
               "This example needs SQLite support. Please read "
               "the Qt SQL driver documentation for information how "
               "to build it."), QMessageBox::Cancel);
        return;
    }

    // Ensure our tables exist (FIXME: not very elegant, this)
    QSqlQuery query(db);
    query.exec("create table video (name varchar(20), path varchar(200))");
    query.exec("create table tag (name varchar(20))");
    query.exec("create table videoTag (tagId int, videoId int)");

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

    m_cachedVideoDirectories.clear();
    QSqlQuery query;
    query.exec("SELECT path FROM video");
    while (query.next())
        m_cachedVideoDirectories << query.value(0).toString();

    scan(QDir(Config::collectionPath));

    reload();
    emit updated();
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
        qDebug() << query.lastQuery();
        query.exec();

        if (dir.exists("tags.txt")) { // There is a tag cache here
            QFile file(dir.filePath("tags.txt"));
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                while (!file.atEnd())
                    addTagToDb(dir.dirName(), QString::fromUtf8(file.readLine())); // One tag per line
            }
        }

    }

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs | QDir::Executable);
                                                                      // QDir::AllDirs ignores name filter


    foreach(QFileInfo subdir, dir.entryInfoList()) {
        scan(QDir(subdir.filePath())); // Recursively scan
    }
}

void Collection::addTagToDb(QString video, QString tag)
{
    tag = tag.simplified(); // Removes spaces at end and beginning, and extra in the middle

    qDebug() << "adding tag: " << tag << " to video: " << video;

    QSqlQuery query;
    query.prepare("INSERT INTO tag(name) VALUES(:tag)");
    query.bindValue(":tag", tag);
    if (!query.exec()) qDebug() << "Tag: " << tag << " exists already";

    query.prepare("SELECT * FROM videoTag WHERE videoId = "
            "(SELECT id FROM video WHERE name = :video) "
            " AND tagId = "
            "(select id FROM tag WHERE name = :tag)");
    query.bindValue(":tag", tag);
    query.bindValue(":video", video);
    query.exec();
    if (query.size() > 0) {
        qDebug() << "Tried to add existing tag to video again";
        return;
    }

    query.prepare("INSERT INTO videoTag(videoId, tagId) VALUES("
            "(SELECT rowid FROM video WHERE name = :video),"
            "(SELECT rowid FROM tag WHERE name = :tag))");
    query.bindValue(":tag", tag);
    query.bindValue(":video", video);
    if (!query.exec())
        qWarning() << "Unable to add tag '" << tag << "' to video '" << video << "'.";
}

void Collection::addTag(const QString &video, const QString &tag)
{
    addTagToDb(video, tag);

    QSqlQuery query;
    query.prepare("SELECT path FROM video WHERE name = :name");
    query.bindValue(":name", video);
    query.exec();
    query.first();
    QString path = query.value(0).toString();

    QFile file(path + "/tags.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        file.write(tag.toUtf8() + "\n");
    }
}

void Collection::reload()
{
    setQuery("SELECT video.name, group_concat(tag.name, ', ') "
                           "FROM video, tag, videoTag "
                           "WHERE videoTag.videoId = video.rowid AND videoTag.tagId = tag.rowid "
                           "GROUP BY videoId");
}

QStringList Collection::getTags(const QString &videoName)
{
    QSqlQuery query;
    query.prepare("SELECT tag.name FROM video, tag, videoTag "
                  "WHERE videoTag.videoId = video.rowid "
                  "AND videoTag.tagId = tag.rowid "
                  "AND video.name = :videoName");
    query.bindValue(":videoName", videoName);
    query.exec();

    QStringList tags;
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
