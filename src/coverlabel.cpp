/*
 * A special label that displays a video cover
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

#include "coverlabel.h"
#include "collection.h"
#include "video.h"
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

CoverLabel::CoverLabel(QWidget *parent) :
    QLabel(parent),
    m_dir(0)
{
    setCursor(Qt::PointingHandCursor);
    connect(&m_timer, SIGNAL(timeout()), SLOT(nextImage()));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

CoverLabel::~CoverLabel()
{
    delete m_dir;
}

void CoverLabel::setVideo(const QString &video)
{
    m_videoName = video;
    setPixmap(Collection::getCover(m_videoName, width()-10));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_path = Collection::getPath(m_videoName);
}

void CoverLabel::leaveEvent(QEvent *)
{
    m_timer.stop();
    setPixmap(Collection::getCover(m_videoName, width()-10));
}

void CoverLabel::enterEvent(QEvent *)
{
    if (m_timer.isActive())
        return; // WTF

    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.png";
    delete m_dir;
    m_dir = new QDirIterator(m_path, nameFilters, QDir::Files, QDirIterator::NoIteratorFlags);
    m_timer.setInterval(0);
    m_timer.start();
}

void CoverLabel::nextImage()
{
    QImage image;

    {
        int height = 0, width = 0, ratio = 0;
        // Sanity check sizes
        do {

            m_dir->next();

            // Skip files larger than a megabyte
            if (m_dir->fileInfo().size() > 1024*1024)
                continue;

            image = QImage(m_dir->filePath());
            height = image.height();
            width = image.width();
            if (height == 0 || width == 0) continue;

            ratio = qMax(width, height) / qMin(width, height);
        } while (m_dir->hasNext() &&
                 (height > 1024 || width > 1024 ||
                  height < 100 || width < 100 ||
                  ratio > 2));
    }
    if (!m_dir->hasNext()) { // restart
        m_timer.stop();
        enterEvent();
        return;
    }

    float maxSize = width() - 10;
    if (image.width() > maxSize) {
        float factor = maxSize / image.width();
        image = Video::quickScale(image, image.width() * factor, image.height() * factor);
    }
    setPixmap(QPixmap::fromImage(image));
    m_timer.setInterval(1000);
}

void CoverLabel::mousePressEvent(QMouseEvent *)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_dir->filePath()));
}
