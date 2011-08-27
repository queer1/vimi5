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
