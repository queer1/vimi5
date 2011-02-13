#include "video.h"
#include "config.h"
#include <QDir>
#include <QDebug>
#include <QPixmap>
#include <QSet>

Video::Video(QString path) :
        m_path(path)
{
    if (path == "") {
        Q_ASSERT(false);
    }

    QDir dir(m_path);

    m_name = dir.dirName();

    if (dir.exists("tags.txt")) { // There is a tag cache here
        QFile file(dir.filePath("tags.txt"));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            while (!file.atEnd())
                m_tags.insert(QString::fromUtf8(file.readLine().simplified().toLower())); // One tag per line
        }
        m_tagList = QStringList(m_tags.toList()).join(", ");
    }
    scanForCovers();
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
    m_tags.insert(tag);
    writeTagCache();

    m_tagList = QStringList(m_tags.toList()).join(", ");
}

void Video::removeTag(QString tag)
{
    m_tags.remove(tag);
    writeTagCache();

    m_tagList = QStringList(m_tags.toList()).join(", ");
}

bool Video::matchesTags(const QSet<QString> &tags) const
{
    return m_tags.contains(tags);
}

QPixmap Video::cover(int maxSize) const
{
    if (m_name.isEmpty())
        return QPixmap();

    QImage cover = m_cover;

    if (cover.height() > maxSize) {
        float factor = (float)maxSize / cover.height();//, cover.width();
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
        QByteArray data = QStringList(m_tags.toList()).join("\n").toUtf8() + "\n";
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
        m_cover = QImage(dir.entryInfoList(QStringList("*front*.jpg")).first().absoluteFilePath());
    else if (dir.entryInfoList(QStringList("*cover*.jpg")).count() > 0)
        m_cover = QImage(dir.entryInfoList(QStringList("*cover*.jpg")).first().absoluteFilePath());
    else if (dir.entryInfoList(QStringList("*" + m_name + "*.jpg")).count() > 0)
        m_cover = QImage(dir.entryInfoList(QStringList("*" + m_name + "*.jpg")).first().absoluteFilePath());
    else if (dir.entryInfoList(QStringList("*.jpg")).count() > 0)
        m_cover = QImage(dir.entryInfoList(QStringList("*.jpg")).first().absoluteFilePath());
    else
        m_cover = QImage(":/images/defaultcover.png");
}
