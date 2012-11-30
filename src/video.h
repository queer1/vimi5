/*
 * Class that holds information about a single video
 * Copyright (C) 2011-2012 cwk <coolwk@gmail.com>
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

#ifndef VIDEO_H
#define VIDEO_H
#include <QStringList>
#include <QImage>
#include <QPixmap>
#include <QObject>
#include <QMutex>

class QString;
class QDataStream;
class Collection;

class Video : public QObject
{
    Q_OBJECT
public:
    Video (Collection *parent, QString path, QString tags, QString coverPath);
    ~Video();

    QPixmap cover(int maxSize);
    const QPixmap &thumbnail();
    //const QImage cover();
    QString coverPath() const { return m_coverPath; }
    QString path() const { return m_path; }
    QString name() const { return m_name; }
    QString tagList() const { return m_tagList; }

    QStringList files() const;
    const QStringList &tags() const { return m_tags; }
    void addTag(QString tag);
    void removeTag(QString tag);
    static QString scanForCovers(QString path);
    static QImage quickScale(const QImage &source, int width, int height);
    static Video *makeVideo (Collection *parent, QString path = "");
    void generateThumbnail();
    void rescanForCovers() { m_coverPath = scanForCovers(m_path); }

signals:
    void coverLoaded(const QString& name);
    void needToLoadCover(Video *me);

private:
    void writeTagCache();

    QString m_path;
    QString m_name;
    QString m_coverPath;
    QStringList m_tags;
    QString m_tagList;
    QImage m_thumbnailImage;
    QPixmap m_thumbnail;
    Collection *m_collection;
    QImage *m_cover;

    static QPixmap *m_defaultCover;
    static QPixmap *m_defaultThumbnail;

    QMutex m_mutex;
};

#endif // VIDEO_H
