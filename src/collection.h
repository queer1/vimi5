// Copyright 2009 cwk

#ifndef COLLECTION_H
#define COLLECTION_H

#include "config.h"
#include <QDir>
#include <QImage>
#include <QSqlQueryModel>
#include <QHash>

class Collection : public QSqlQueryModel
{

    Q_OBJECT;

public:
    Collection();

    void addTag(const QString &video, const QString &tag);
    void removeTag(const QString &video, const QString &tag);
    static QStringList getTags(const QString& videoName = QString());
    static QStringList getFiles(const QString& videoName);
    static QString getPath(const QString &videoName);
    static QPixmap getCover(const QString &videoName, int maxSize = Config::maxCoverSize());
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;

signals:
    void updated();
    void statusUpdated(const QString &text);

public slots:
    void rescan();

private slots:
    void reload(); // Reloads from the database

private:
    void scan(QDir directory);
    static void writeTagCache(const QString &video);

    static void addTagToDb(QString video, QString tag);
    static void initializeDatabase();
    static QImage quickScale(const QImage &source, int width, int height);

    static QHash<QString, QImage> m_coverCache;

    QStringList m_cachedVideoDirectories;
};

#endif // COLLECTION_H
