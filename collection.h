// Copyright 2009 cwk

#ifndef COLLECTION_H
#define COLLECTION_H

#include "config.h"
#include <QDir>
#include <QSqlQueryModel>

class Collection : public QSqlQueryModel
{

    Q_OBJECT;

public:
    Collection();

    void addTag(const QString &video, const QString &tag);
    QStringList getTags(const QString& videoName);
    QStringList getFiles(const QString& videoName);
    QString getPath(const QString &videoName);

signals:
    void updated();

public slots:
    void rescan();

private slots:
    void reload(); // Reloads from the database

private:
    void scan(QDir directory);
    void addTagToDb(QString video, QString tag);


    QStringList m_cachedVideoDirectories;
};

#endif // COLLECTION_H
