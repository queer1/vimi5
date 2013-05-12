/*
 * Dialog to create covers from video files
 * Copyright (C) 2010-2012 cwk <coolwk@gmail.com>
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

#ifndef COVERMAKER_H
#define COVERMAKER_H

#include <QDialog>

class QComboBox;
class VideoWidget;
class QSlider;
class QScrollBar;

class CoverMaker : public QDialog
{
    Q_OBJECT
public:
    CoverMaker(QString videoName, QWidget *parent = 0);

signals:
    void coverCreated(QString name);

private slots:
    void selectVideo(QString);
    void saveFrame();

private:
    QComboBox *m_comboBox;
    VideoWidget *m_videoWidget;
    QScrollBar *m_slider;
    QString m_path;
    QString m_name;
};


#endif//COVERMAKER_H
