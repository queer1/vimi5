#include "video.h"
#include "config.h"
#include "mainwindow.h"
#include "collection.h"
#include <QDir>
#include <QDebug>
#include <QPixmap>
#include <QSet>
#include <QWaitCondition>
#include <QtCore/QtConcurrentRun>
#include <cstdlib>

QPixmap *Video::m_defaultCover = 0;

Video::Video(QObject *parent, QString path) : QObject(parent),
        m_path(path)
{
    Q_ASSERT(path != "");

    QDir dir(m_path);

    m_name = dir.dirName();

   if (dir.exists("tags.txt")) { // There is a tag cache here
        QFile file(dir.filePath("tags.txt"));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            while (!file.atEnd())
                m_tags.append(QString::fromUtf8(file.readLine().simplified().toLower())); // One tag per line
        }
        qSort(m_tags);
        m_tagList = m_tags.join(", ");
    }
    scanForCovers();
}

Video::Video(QObject *parent, QString path, QString tags, QString coverPath) : QObject(parent),
    m_path(path),
    m_coverPath(coverPath)
    //m_cover(QImage(m_coverPath))
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
    float factor = (float)128 / m_cover.height();//, cover.width();
    //m_coverIconCache = QPixmap::fromImage(quickScale(m_cover, m_cover.width()*factor, m_cover.height() * factor));
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
}

void Video::removeTag(QString tag)
{
    m_tags.removeAll(tag);
    writeTagCache();

    m_tagList = m_tags.join(", ");
}

QPixmap Video::cover(int maxSize)
{
    if (m_name.isEmpty())
        return QPixmap();

    //QImage cover = m_cover;
    if (m_cover.isNull()) {
        if (!m_coverPath.isEmpty()) {
            QtConcurrent::run(this, &Video::loadCover);
        }
        if (m_defaultCover == 0)
            m_defaultCover = new QPixmap(QPixmap::fromImage(QImage(":/images/defaultcover.png")));
        //m_cover = m_defaultCover->toImage();
        return *m_defaultCover;
    }

    if (m_cover.height() > maxSize) {
        float factor = (float)maxSize / m_cover.height();//, cover.width();
        if (maxSize == 128) {
            if (m_coverIconCache.isNull()) {
                float factor = (float)128 / m_cover.height();//, cover.width();
                m_coverIconCache = QPixmap::fromImage(quickScale(m_cover, m_cover.width()*factor, m_cover.height() * factor));
            }
            return m_coverIconCache;
        }
        return QPixmap::fromImage(quickScale(m_cover, m_cover.width()*factor, m_cover.height() * factor));
    } else
        return QPixmap::fromImage(m_cover);
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

void Video::writeTagCache()
{
    QString filename = m_path + "/tags.txt";
    QFile file(filename + ".tmp");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QByteArray data = m_tags.join("\n").toUtf8() + "\n";
        if (file.write(data) == data.size()) {
            QFile::remove(filename);
            file.rename(filename + ".tmp", filename);
        }
    }
}

void Video::scanForCovers()
{
    QDir dir(m_path);

    // Check if there is a separate folder with covers
    QStringList names;
    names << "*cover*" << "*Cover*";
    dir.setNameFilters(names);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    if (dir.entryList().count() > 0) {
        dir.cd(dir.entryList().first());
    }

    // Try to either find *front*.jpg, *cover*.jpg or just any plain *.jpg
    dir.setFilter(QDir::Files);
    if (dir.entryInfoList(QStringList("*front*.jpg")).count() > 0)
        m_coverPath = dir.entryInfoList(QStringList("*front*.jpg")).first().absoluteFilePath();
    else if (dir.entryInfoList(QStringList("*cover*.jpg")).count() > 0)
        m_coverPath = dir.entryInfoList(QStringList("*cover*.jpg")).first().absoluteFilePath();
    else if (dir.entryInfoList(QStringList("*" + m_name + "*.jpg")).count() > 0)
        m_coverPath = dir.entryInfoList(QStringList("*" + m_name + "*.jpg")).first().absoluteFilePath();
    else if (dir.entryInfoList(QStringList("*.jpg")).count() > 0)
        m_coverPath = dir.entryInfoList(QStringList("*.jpg")).first().absoluteFilePath();
    //else
    //    m_coverPath = ":/images/defaultcover.png";

    //m_cover = QImage(m_coverPath);
    float factor = (float)128 / m_cover.height();//, cover.width();
//    m_coverIconCache = QPixmap::fromImage(quickScale(m_cover, m_cover.width()*factor, m_cover.height() * factor));
}

void Video::loadCover()
{
    if (!MainWindow::running)
        return;
    if (!Collection::launched)
        Collection::launchWaiter.wait(&Collection::launchMutex);

    QImage cover = QImage(m_coverPath);
    if (cover.isNull()) {
        qWarning() << "unable to load cover:" << m_coverPath;
        m_coverPath = "";
    }

    m_cover = cover;
    //qDebug() << "loaded cover: " << m_coverPath;
    emit coverLoaded(m_name);
}
