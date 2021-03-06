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

#include "video.h"
#include "config.h"
#include "mainwindow.h"
#include "collection.h"
#include <QDir>
#include <QDebug>
#include <QPixmap>
#include <QSet>
#include <QWaitCondition>
#include <QtConcurrent>
#include <cstdlib>
#include <QMutexLocker>

QPixmap *Video::m_defaultCover = 0;
QPixmap *Video::m_defaultThumbnail = 0;

Video *Video::makeVideo(Collection *parent, QString path)
{
    Q_ASSERT(path != "");

    QDir dir(path);

    QStringList tags;
    return new Video(parent, path, tags.join(","), scanForCovers(path), -1, QString());
}

Video::Video(Collection *parent, QString path, QString tags, QString coverPath, int lastPosition, QString lastFile) : QObject(parent),
    m_path(path),
    m_coverPath(coverPath),
    m_collection(parent),
    m_cover(0),
    m_lastPosition(lastPosition),
    m_lastFile(lastFile)
{
    QStringList tagList = tags.split(',');
    foreach(QString tag, tagList) {
        if (!tag.isEmpty())
            m_tags.append(tag);
    }
    qSort(m_tags);
    m_tagList = m_tags.join(", ");


    QDir dir(m_path);
    m_name = dir.dirName();

    if (m_defaultThumbnail == 0) {
        QImage cover = QImage(":/images/defaultcover.png");
        float factor = (float)128 / cover.height();//, cover.width();
        m_defaultThumbnail = new QPixmap(QPixmap::fromImage(quickScale(cover, cover.width()*factor, cover.height() * factor)));
    }
    if (m_defaultCover == 0)
        m_defaultCover = new QPixmap(QPixmap::fromImage(QImage(":/images/defaultcover.png")));
}
Video::~Video()
{
    m_mutex.unlock();
    delete m_cover;
}

QStringList Video::files() const
{
    QDir dir(m_path);
    dir.setNameFilters(Config::movieSuffixes());
    dir.setFilter(QDir::Files);

    return dir.entryList();
}

void Video::addTag(QString tag)
{
    m_tags.append(tag);
    qSort(m_tags);
    writeTagCache();

    m_tagList = m_tags.join(", ");
    emit tagsUpdated();
}

void Video::removeTag(QString tag)
{
    m_tags.removeAll(tag);
    writeTagCache();

    m_tagList = m_tags.join(", ");
    emit tagsUpdated();
}

QPixmap Video::cover(int maxSize)
{
    if (m_name.isEmpty())
        return QPixmap();

    //QImage cover = m_cover;
    if (m_cover == 0) {
        if (!m_coverPath.isEmpty()) {
            m_cover = new QImage(m_coverPath);
            if (m_cover->isNull()) {
                delete m_cover;
                m_cover = 0;
                m_coverPath = "";
                return *m_defaultCover;
            }
        } else {
            return *m_defaultCover;
        }
    }

    int size = qMax(m_cover->width(), m_cover->height());

    if (size > maxSize) {
        float factor = (float)maxSize / size;//, cover.width();
        return QPixmap::fromImage(quickScale(*m_cover, m_cover->width()*factor, m_cover->height() * factor));
    } else
        return QPixmap::fromImage(*m_cover);
}



// Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
// for the explanation of the trick, check out:
// http://www.virtualdub.org/blog/pivot/entry.php?id=116
// http://www.compuphase.com/graphic/scale3.htm
#define AVG(a,b)  ( ((((a)^(b)) & 0xfefefefeUL) >> 1) + ((a)&(b)) )

// assume that the source image is ARGB32 formatted
QImage Video::quickScale(const QImage &s, int width, int height)
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

void Video::generateThumbnail()
{
    QMutexLocker l(&m_mutex);

    if (m_coverPath.isEmpty())
        return;

    QImage cover(m_coverPath);
    if (cover.isNull()) {
        qWarning() << "unable to load cover:" << m_coverPath;
        m_coverPath = "";
    } else {
        float factor = (float)128 / cover.height();//, cover.width();
        m_thumbnailImage = quickScale(cover, cover.width()*factor, cover.height() * factor);
    }
    emit coverLoaded(m_name);
}

const QPixmap &Video::thumbnail()
{

    if (m_thumbnail.isNull()) {
        if (m_thumbnailImage.isNull()) {
            emit needToLoadCover(this);
            return *m_defaultThumbnail;
        } else {
            m_thumbnail = QPixmap::fromImage(m_thumbnailImage);
            m_thumbnailImage = QImage();
        }
    }

    return m_thumbnail;
}
