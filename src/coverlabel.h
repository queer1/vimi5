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

#ifndef COVERLABEL_H
#define COVERLABEL_H

#include <QLabel>
#include <QDirIterator>
#include <QTimer>

class CoverLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CoverLabel(QWidget *parent = 0);
    ~CoverLabel();
    void setVideo(const QString &video);

private slots:
    void nextImage();

private:
    void leaveEvent(QEvent *);
    void enterEvent(QEvent * = 0);
    void mousePressEvent(QMouseEvent *ev);
    void updateGeometry() {}

    QString m_videoName;
    QString m_path;
    QDirIterator *m_dir;
    QTimer m_timer;
};

#endif // COVERLABEL_H
