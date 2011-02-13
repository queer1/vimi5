#ifndef COVERMAKER_H
#define COVERMAKER_H

#include <QWidget>

extern "C" {

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    VideoWidget(QString file);
    ~VideoWidget();
    void seek(int seconds);
    int length() { return static_cast<int>(m_pFormatContext->duration / AV_TIME_BASE); }
    QImage getFrame();
    void paintEvent(QPaintEvent *);

private:
    bool decodeVideoPacket();
    bool getVideoPacket();
    void decodeVideoFrame();

    AVFormatContext *m_formatContext;
    AVFrame *m_frame;
    AVPacket *m_packet;
    int m_videoStream;
    AVCodecContext *m_videoCodecContext;
};

#endif // COVERMAKER_H
