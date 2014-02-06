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

extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

VideoFrameDumper::VideoFrameDumper(QUrl path) : fmt_ctx(0)
{
    QFileInfo fileInfo(path.toLocalFile());
    if (!fileInfo.exists()) {
        qWarning() << "can't open file" << path.toLocalFile();
        return;
    }

    m_outputPath = fileInfo.canonicalPath();
    m_outputFile = fileInfo.absoluteFilePath().toLocal8Bit();
    m_filename = fileInfo.fileName();

    av_register_all();

    if (avformat_open_input(&fmt_ctx, m_outputFile.constData(), NULL, NULL) < 0) {
        qWarning() << "Unable to open input" << m_outputFile;
        return;
    }

    if (avformat_find_stream_info(fmt_ctx, 0) < 0) {
        qWarning() << "unable to get stream info";
        return;
    }

    video_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (video_stream_idx < 0) {
        qWarning() << "unable to open codec context";
        return;
    }
    dec = avcodec_find_decoder(fmt_ctx->streams[video_stream_idx]->codec->codec_id);
    if (!dec) {
        qWarning() << "Unable to find decoder!";
        return;
    }
    if (avcodec_open2(fmt_ctx->streams[video_stream_idx]->codec, dec, 0) < 0) {
        qWarning() << "Unable to initialize codec context";
        return;
    }

    //av_dump_format(fmt_ctx, 0, m_filename.constData(), 0);

    frame = av_frame_alloc();

    pkt = new AVPacket;
    av_init_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;

    moveToThread(&m_thread);
    m_thread.start();
}

VideoFrameDumper::~VideoFrameDumper()
{
    if (!fmt_ctx) return;

    avcodec_close(fmt_ctx->streams[video_stream_idx]->codec);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    free(pkt);
    emit screenshotsCreated(m_outputPath);
    deleteLater();
}

void VideoFrameDumper::saveFrameToImage(QString outFile)
{
    AVCodecContext *video_dec_ctx = fmt_ctx->streams[video_stream_idx]->codec;

    //static SwsContext* scaleContext = 0;
    const int outHeight = video_dec_ctx->height;
    const int outWidth = video_dec_ctx->width;
    //const int outWidth = 1000;
    static SwsContext* scaleContext=0;
    scaleContext = sws_getCachedContext(scaleContext,
                                              video_dec_ctx->width, video_dec_ctx->height, video_dec_ctx->pix_fmt,
                                              outWidth, outHeight, PIX_FMT_RGB24,
                                              SWS_BILINEAR, NULL, NULL, NULL);

    AVFrame *avFrame = avcodec_alloc_frame();

    int numBytes = avpicture_get_size(PIX_FMT_RGB24, video_dec_ctx->width, video_dec_ctx->height);
    quint8 *frameBuffer = reinterpret_cast<quint8*>(av_malloc(numBytes));

    avpicture_fill((AVPicture*) avFrame, frameBuffer, PIX_FMT_RGB24, outWidth, outHeight);

    sws_scale(scaleContext, frame->data, frame->linesize, 0, video_dec_ctx->height,
              avFrame->data, avFrame->linesize);
    //sws_freeContext(scaleContext);

    QImage image(outWidth, outHeight, QImage::Format_RGB32);
    //memcpy(image.bits(), &avFrame->data[0][0], image.byteCount());

    uint8_t *src = (uint8_t *)(avFrame->data[0]);
    for (int y = 0; y < outHeight; y++)
    {
        QRgb *scanLine = (QRgb *) image.scanLine(y);
        for (int x = 0; x < outWidth; x++)
        {
            scanLine[x] = qRgb(src[3*x], src[3*x+1], src[3*x+2]);
        }
        src += avFrame->linesize[0];
    }

    //QImage image(&avFrame->data[0][0], video_dec_ctx->width, video_dec_ctx->height, QImage::Format_RGB888);

    if (!image.save(outFile))
        qWarning() << "Unable to save file" << outFile;

    av_free(avFrame);
    av_free(frameBuffer);
}

void VideoFrameDumper::seek(quint64 pos)
{
    if (!fmt_ctx) return;

    if (pos == -1) {
        av_seek_frame(fmt_ctx, -1, fmt_ctx->duration / 2, 0);
    } else {
        av_seek_frame(fmt_ctx, -1, pos, 0);
    }
    avcodec_flush_buffers(fmt_ctx->streams[video_stream_idx]->codec);
}

void VideoFrameDumper::createSnapshots(int num)
{
    if (!fmt_ctx) return;

    int64_t skip_size = (fmt_ctx->duration) / num;
    int i = 0;
    int ret;
    while (true) {
        ret = av_read_frame(fmt_ctx, pkt);

        if (ret == AVERROR(EAGAIN))
            continue;

        if (ret < 0)
            break;

        if (pkt->stream_index != video_stream_idx) {
            av_free_packet(pkt);
            continue;
        }

        int got_frame = 0;
        ret = avcodec_decode_video2(fmt_ctx->streams[video_stream_idx]->codec, frame, &got_frame, pkt);
        if (ret < 0) {
            char errbuf[1024];
            av_strerror(ret, errbuf, 1024);
            qWarning() << "error decoding:" << errbuf << ret;
            break;
        }
        if (got_frame){
            if (num == -1) {
                saveFrameToImage(m_outputPath + "/.vimicover.jpg");
                emit coverCreated(m_outputPath);
            } else {
                saveFrameToImage(m_outputPath + "/.vimiframe_" + QString::number(i*skip_size/1000) + "_" + m_filename + ".jpg");
                emit statusUpdated("Creating snapshots: " + QString::number(i) + '%');
            }


            i++;
            if (i>num)
                break;


            av_seek_frame(fmt_ctx, -1, skip_size*i, 0);
            avcodec_flush_buffers(fmt_ctx->streams[video_stream_idx]->codec);
        }
        av_free_packet(pkt);
    }

    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, 1024);
        qWarning() << "error reading frame:" << errbuf << "(" << ret << ")";
    }

    if (num > 0) {
        emit screenshotsCreated(m_outputPath);
    }

//    deleteLater();
}
