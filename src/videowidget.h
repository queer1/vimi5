/*
 * Custom video widget that uses ffmpeg to decode and display video frames
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

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
class AVFormatContext;
class AVFrame;
class AVPacket;
class AVCodecContext;

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    VideoWidget(QWidget *parent, QString file);
    ~VideoWidget();
    int length();
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;
    QImage getFrame() const { return m_activeFrame; }

public slots:
    void seek(int seconds);


private:
    bool decodeVideoPacket();
    bool getVideoPacket();
    void decodeVideoFrame();

    AVFormatContext *m_formatContext;
    AVFrame *m_frame;
    AVPacket *m_packet;
    int m_videoStream;
    AVCodecContext *m_videoCodecContext;
    QImage m_activeFrame;
};

#endif // VIDEOWIDGET_H
