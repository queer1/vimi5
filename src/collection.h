// Copyright 2009 cwk

#ifndef COLLECTION_H
#define COLLECTION_H

#include "config.h"
#include "video.h"
#include <QDir>
#include <QImage>
#include <QAbstractTableModel>
#include <QHash>
#include <QSet>

class Collection : public QAbstractTableModel
{

    Q_OBJECT;

public:
    Collection();
    ~Collection();

    void addTag(const QString &video, const QString &tag);
    void removeTag(const QString &video, const QString &tag);
    static QSet<QString> getTags(const QString& videoName = QString());
    static QStringList getFiles(const QString& videoName);
    static QString getPath(const QString &videoName);
    static QPixmap getCover(const QString &videoName, int maxSize = Config::maxCoverSize());
    static void scanForCovers(const QString &videoName);

    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & = QModelIndex()) const { return m_videoNames.size(); }
    int columnCount(const QModelIndex &) const { return 3; }
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:
    void updated();
    void statusUpdated(const QString &text);

public slots:
    void rescan();

private slots:
    void reload(); // Reloads from the database

private:
    void scan(QDir directory);
    void addVideo(const Video &video);

    static void initializeDatabase();

    static QHash<QString, Video> m_videos;
    static QStringList m_videoNames;

    QStringList m_cachedVideoDirectories;
};

#endif // COLLECTION_H
