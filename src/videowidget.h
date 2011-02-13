#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>

extern "C" {

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    VideoWidget(QWidget *parent, QString file);
    ~VideoWidget();
    int length() { return static_cast<int>(m_formatContext->duration *2 / AV_TIME_BASE); }
    QImage getFrame();
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;

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
