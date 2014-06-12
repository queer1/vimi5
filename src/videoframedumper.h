/*
 * Video frame dumper
 * Copyright (C) 2013-2014 cwk <coolwk@gmail.com>
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

#ifndef VIDEOFRAMEDUMPER_H
#define VIDEOFRAMEDUMPER_H

#include <QObject>
#include <QThread>
#include <QUrl>
#include <QRunnable>

class AVFormatContext;
class AVCodec;
class AVFrame;
class AVPacket;

class VideoFrameDumper : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit VideoFrameDumper(QUrl path);
    ~VideoFrameDumper();

    void setNum(int num) { m_amount = num; }

public slots:
    void run();
    void seek(qint64 pos);

signals:
    void screenshotsCreated(QString path);
    void coverCreated(QString path, QString coverPath);
    void statusUpdated(QString status);
    void error(QString message);


private:
    void saveFrameToImage(QString outFile);

    QString m_outputPath;
    QString m_filename;
    QByteArray m_outputFile;
    int m_amount;

    AVFormatContext *m_formatContext;
    AVCodec *m_decoder;
    AVFrame *m_frame;
    AVPacket *m_packet;
    int m_videoStreamIndex;
};

#endif // VIDEOFRAMEDUMPER_H
