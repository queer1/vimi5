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

#include "videoframedumper.h"
#include <QFileInfo>
#include <QDebug>
#include <QImage>
#include <QMutex>

extern "C" {
#define __STDC_CONSTANT_MACROS
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

VideoFrameDumper::VideoFrameDumper(QUrl path) : m_amount(100), m_formatContext(0)
{
    QFileInfo fileInfo(path.toLocalFile());
    if (!fileInfo.exists()) {
        qWarning() << "VFD: can't open file" << path.toLocalFile();
        return;
    }

    m_outputPath = fileInfo.canonicalPath();
    m_outputFile = fileInfo.absoluteFilePath().toLocal8Bit();
    m_filename = fileInfo.fileName();

    qDebug() << "created VFD:" << path;
}

VideoFrameDumper::~VideoFrameDumper()
{
    if (!m_formatContext) return;

    avcodec_close(m_formatContext->streams[m_videoStreamIndex]->codec);
    avformat_close_input(&m_formatContext);
    av_frame_free(&m_frame); //FIXME this crashes
    free(m_packet);
    emit screenshotsCreated(m_outputPath);
    deleteLater();
}

void VideoFrameDumper::saveFrameToImage(QString outFile)
{
    static QMutex swsMutex;
    swsMutex.lock();
    AVCodecContext *video_dec_ctx = m_formatContext->streams[m_videoStreamIndex]->codec;

    const int outHeight = video_dec_ctx->height;
    const int outWidth = video_dec_ctx->width;
    static SwsContext* scaleContext=0;


    scaleContext = sws_getCachedContext(scaleContext,
                                              video_dec_ctx->width, video_dec_ctx->height, video_dec_ctx->pix_fmt,
                                              outWidth, outHeight, PIX_FMT_RGB24,
                                              SWS_BILINEAR, NULL, NULL, NULL);

    AVFrame *avFrame = av_frame_alloc();

    int numBytes = avpicture_get_size(PIX_FMT_RGB24, video_dec_ctx->width, video_dec_ctx->height);
    quint8 *frameBuffer = reinterpret_cast<quint8*>(av_malloc(numBytes));

    avpicture_fill((AVPicture*) avFrame, frameBuffer, PIX_FMT_RGB24, outWidth, outHeight);

    sws_scale(scaleContext, m_frame->data, m_frame->linesize, 0, video_dec_ctx->height,
              avFrame->data, avFrame->linesize);

    QImage image(outWidth, outHeight, QImage::Format_RGB32);

    uint8_t *src = (uint8_t *)(avFrame->data[0]);
    for (int y = 0; y < outHeight; y++) {
        QRgb *scanLine = (QRgb *) image.scanLine(y);
        for (int x = 0; x < outWidth; x++) {
            scanLine[x] = qRgb(src[3*x], src[3*x+1], src[3*x+2]);
        }
        src += avFrame->linesize[0];
    }

    if (!image.save(outFile)) {
        qWarning() << "Unable to save file" << outFile;
    }

    av_free(avFrame);
    av_free(frameBuffer);
    swsMutex.unlock();
}

void VideoFrameDumper::seek(qint64 pos)
{
    if (!m_formatContext) return;

    if (pos == -1) {
        av_seek_frame(m_formatContext, -1, m_formatContext->duration / 2, 0);
    } else {
        av_seek_frame(m_formatContext, -1, pos, 0);
    }
    avcodec_flush_buffers(m_formatContext->streams[m_videoStreamIndex]->codec);
}

void VideoFrameDumper::run()
{
    if (avformat_open_input(&m_formatContext, m_outputFile.constData(), NULL, NULL) < 0) {
        qWarning() << "VFD: Unable to open input" << m_outputFile;
        return;
    }

    if (avformat_find_stream_info(m_formatContext, 0) < 0) {
        qWarning() << "VFD: unable to get stream info";
        return;
    }

    m_videoStreamIndex = av_find_best_stream(m_formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (m_videoStreamIndex < 0) {
        qWarning() << "VFD: unable to open codec context";
        return;
    }
    m_decoder = avcodec_find_decoder(m_formatContext->streams[m_videoStreamIndex]->codec->codec_id);
    if (!m_decoder) {
        qWarning() << "VFD: Unable to find decoder!";
        return;
    }
    if (avcodec_open2(m_formatContext->streams[m_videoStreamIndex]->codec, m_decoder, 0) < 0) {
        qWarning() << "VFD: Unable to initialize codec context";
        return;
    }

    m_frame = av_frame_alloc();

    m_packet = new AVPacket;
    av_init_packet(m_packet);
    m_packet->data = NULL;
    m_packet->size = 0;

    qDebug() << "creating for" << m_outputFile;
    if (!m_formatContext) {
        emit error("Error while creating snapshots");
        return;
    }

    int64_t skip_size = (m_formatContext->duration) / m_amount;
    int i = 0;
    int ret;
    while (true) {
        ret = av_read_frame(m_formatContext, m_packet);

        if (ret == AVERROR(EAGAIN))
            continue;

        if (ret < 0)
            break;

        if (m_packet->stream_index != m_videoStreamIndex) {
            av_free_packet(m_packet);
            continue;
        }

        int got_frame = 0;
        ret = avcodec_decode_video2(m_formatContext->streams[m_videoStreamIndex]->codec, m_frame, &got_frame, m_packet);
        if (ret < 0) {
            char errbuf[1024];
            av_strerror(ret, errbuf, 1024);
            qWarning() << "error decoding:" << errbuf << ret;
            break;
        }
        if (got_frame){
            if (m_amount == -1) {
                QString coverPath(m_outputPath + "/.vimicover.jpg");
                QFileInfo fileInfo(coverPath);
                int filePostfix = 0;
                while (fileInfo.exists()) {
                    fileInfo.setFile(m_outputPath + "/.vimicover" + QString::number(filePostfix) + ".jpg");
                }
                saveFrameToImage(coverPath);
                emit coverCreated(m_outputPath, coverPath);
            } else {
                saveFrameToImage(m_outputPath + "/.vimiframe_" + QString::number(i*skip_size/1000) + "_" + m_filename + ".jpg");
                emit statusUpdated("Creating snapshots: " + QString::number(i) + '%');
            }


            i++;
            if (i>m_amount)
                break;


            av_seek_frame(m_formatContext, -1, skip_size*i, 0);
            avcodec_flush_buffers(m_formatContext->streams[m_videoStreamIndex]->codec);
        }
        av_free_packet(m_packet);
    }

    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, 1024);
        qWarning() << "error reading frame:" << errbuf << "(" << ret << ")";
    }

    if (m_amount > 0) {
        emit screenshotsCreated(m_outputPath);
    }
}
