#ifndef VIDEOFRAMEDUMPER_H
#define VIDEOFRAMEDUMPER_H

#include <QObject>
#include <QThread>
#include <QUrl>

class AVFormatContext;
class AVCodec;
class AVFrame;
class AVPacket;

class VideoFrameDumper : public QObject
{
    Q_OBJECT
public:
    explicit VideoFrameDumper(QUrl path);
    ~VideoFrameDumper();

public slots:
    void createSnapshots(int num=100);
    void seek(quint64 pos);

signals:
    void screenshotsCreated(QString path);
    void coverCreated(QString path);
    void statusUpdated(QString status);


private:
    void saveFrameToImage(QString outFile);

    QThread m_thread;
    QString m_outputPath;
    QString m_filename;
    QByteArray m_outputFile;

    AVFormatContext *fmt_ctx;
    AVCodec *dec;
    AVFrame *frame;
    AVPacket *pkt;
    int video_stream_idx;
};

#endif // VIDEOFRAMEDUMPER_H
