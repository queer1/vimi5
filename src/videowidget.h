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
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;
    QImage getFrame() const { return m_activeFrame; }

public slots:
    void seek(int seconds);


private:
    bool decodeVideoPacket();
    bool getVideoPacket();
    QImage decodeVideoFrame();

    AVFormatContext *m_formatContext;
    AVFrame *m_frame;
    AVPacket *m_packet;
    int m_videoStream;
    AVCodecContext *m_videoCodecContext;
    QImage m_activeFrame;
};

#endif // VIDEOWIDGET_H
