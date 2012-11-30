/*
 * Helper class to load covers in a separate thread
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

#ifndef COVERLOADER_H
#define COVERLOADER_H

#include <QObject>
#include <QList>
#include <QMutex>
#include <QWaitCondition>

class Video;
class CoverLoader : public QObject
{
    Q_OBJECT
public:
    explicit CoverLoader(QObject *parent = 0);
    bool running;

public slots:
    void loadVideo(Video *video);
};

#endif // COVERLOADER_H
