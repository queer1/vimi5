#include "covermaker.h"
#include <QFileInfo>
#include <QDebug>
#include <QPainter>

#include "videowidget.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

}

VideoWidget::VideoWidget(QWidget *parent, QString file)
    : QWidget(parent),
    m_packet(0),
    m_videoStream(-1)
{
    av_register_all();
    avcodec_init();
    avcodec_register_all();

    QFileInfo fileInfo(file);
    m_formatContext = avformat_alloc_context();

    qWarning() << fileInfo.absoluteFilePath().toUtf8().data();
    if (avformat_open_input(&m_formatContext, fileInfo.absoluteFilePath().toUtf8().constData(), NULL, NULL) != 0) {
        qWarning() <<  "Could not open input file: " << fileInfo.absoluteFilePath();
        return;
    }

    if (av_find_stream_info(m_formatContext) < 0) {
        qWarning() << "Could not find stream information";
        return;
    }

    for (unsigned int i = 0; i < m_formatContext->nb_streams; i++) {
        if (m_formatContext->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) {
            m_videoStream = i;
            break;
        }
    }

    if (m_videoStream == -1) {
        qWarning() << "Could not find video stream";
        return;
    }

    m_videoCodecContext = m_formatContext->streams[m_videoStream]->codec;
    AVCodec *videoCodec = avcodec_find_decoder(m_videoCodecContext->codec_id);

    if (videoCodec == NULL) {
        // set to NULL, otherwise avcodec_close(m_pVideoCodecContext) crashes
        m_videoCodecContext = NULL;
        qWarning() << "Video Codec not found";
        return;
    }

    // Inform the codec that we can handle truncated bitstreams -- i.e.,
    // bitstreams where frame boundaries can fall in the middle of packets
//    if(videoCodec->capabilities & CODEC_CAP_TRUNCATED)
//        m_videoCodecContext->flags|=CODEC_FLAG_TRUNCATED;

    m_videoCodecContext->workaround_bugs = 1;

    if (avcodec_open(m_videoCodecContext, videoCodec) < 0) {
        qWarning() << "Could not open video codec";
    }

    m_frame = avcodec_alloc_frame();
    Q_ASSERT(m_frame);
    decodeVideoFrame();
    seek(length() / 2);
}

VideoWidget::~VideoWidget()
{
    if (m_videoCodecContext)
        avcodec_close(m_videoCodecContext);

    if (m_formatContext)
        avformat_free_context(m_formatContext);

    if (m_packet) {
        av_free_packet(m_packet);
        //delete m_packet;
    }

    if (m_frame)
        av_free(m_frame);
}

void VideoWidget::seek(int halfSeconds)
{
    qint64 timestamp = (AV_TIME_BASE/100) * static_cast<qint64>(halfSeconds);
    if (timestamp < 0) {
        timestamp = 0;
    }


    int ret = av_seek_frame(m_formatContext, -1, timestamp, 0);
    if (ret >= 0) {
        avcodec_flush_buffers(m_formatContext->streams[m_videoStream]->codec);
    } else {
        qWarning() << "Seeking in video failed";
        return;
    }

    int keyFrameAttempts = 0;
    bool gotFrame = false;

    do {
        int count = 0;
        gotFrame = 0;

        while (!gotFrame && count < 20) {
            getVideoPacket();
            gotFrame = decodeVideoPacket();
            ++count;
        }

        ++keyFrameAttempts;
    } while ((!gotFrame || !m_frame->key_frame) && keyFrameAttempts < 200);

    if (gotFrame == 0) {
        qWarning() << "Seeking in video failed";
    }
    decodeVideoFrame();
    repaint();
}

bool VideoWidget::decodeVideoPacket()
{
    if (m_packet->stream_index != m_videoStream) {
        return false;
    }

    avcodec_get_frame_defaults(m_frame);

    int frameFinished = 0;

    int bytesDecoded = avcodec_decode_video2(m_videoCodecContext, m_frame, &frameFinished, m_packet);

    if (bytesDecoded < 0) {
        qWarning() << "Failed to decode video packet: bytesDecoded < 0";
    }

    return (frameFinished > 0);
}


bool VideoWidget::getVideoPacket()
{
    bool framesAvailable = true;
    bool frameDecoded = false;

    int attempts = 0;

    if (m_packet) {
        av_free_packet(m_packet);
        delete m_packet;
    }

    m_packet = new AVPacket();

    while (framesAvailable && !frameDecoded && (attempts++ < 1000)) {
        framesAvailable = av_read_frame(m_formatContext, m_packet) >= 0;
        if (framesAvailable) {
            frameDecoded = m_packet->stream_index == m_videoStream;
            if (!frameDecoded) {
                av_free_packet(m_packet);
            }
        }
    }

    return frameDecoded;
}

void VideoWidget::decodeVideoFrame()
{
    bool frameFinished = false;

    while (!frameFinished && getVideoPacket()) {
        frameFinished = decodeVideoPacket();
    }

    if (!frameFinished) {
        qWarning() << "decodeVideoFrame() failed: frame not finished";
        return;
    }

    if (m_frame->interlaced_frame) {
        avpicture_deinterlace((AVPicture*) m_frame, (AVPicture*) m_frame, m_videoCodecContext->pix_fmt,
                              m_videoCodecContext->width, m_videoCodecContext->height);
    }

    {
        SwsContext* scaleContext = sws_getContext(m_videoCodecContext->width, m_videoCodecContext->height,
                                                  m_videoCodecContext->pix_fmt,
                                                  m_videoCodecContext->width, m_videoCodecContext->height,
                                                  PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

        AVFrame *avFrame = avcodec_alloc_frame();

        int numBytes = avpicture_get_size(PIX_FMT_RGB24, m_videoCodecContext->width, m_videoCodecContext->height);
        quint8 *frameBuffer = reinterpret_cast<quint8*>(av_malloc(numBytes));

        avpicture_fill((AVPicture*) avFrame, frameBuffer, PIX_FMT_RGB24, m_videoCodecContext->width, m_videoCodecContext->height);

        sws_scale(scaleContext, m_frame->data, m_frame->linesize, 0, m_videoCodecContext->height,
                  avFrame->data, avFrame->linesize);
        sws_freeContext(scaleContext);

        av_free(m_frame);
       // av_free(frameBuffer);

        m_frame = avFrame;
    }

    QImage frame(m_videoCodecContext->width, m_videoCodecContext->height, QImage::Format_RGB888);

    //memcpy(frame.bits(), &m_frame[0], m_frame->linesize[m_frame->height] * m_frame->height);

    for (int y=0; y<m_videoCodecContext->height; y++) {
        memcpy(frame.scanLine(y), &m_frame->data[0][y*m_frame->linesize[0]], m_frame->linesize[0]);
    }

    m_activeFrame = frame;

}

void VideoWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(rect(), m_activeFrame);
}

QSize VideoWidget::sizeHint() const
{
    return QSize(m_videoCodecContext->width, m_videoCodecContext->height);
}

int VideoWidget::length()
{
    return static_cast<int>(m_formatContext->duration * 100 / AV_TIME_BASE);
}
