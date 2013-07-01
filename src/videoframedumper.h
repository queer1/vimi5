#ifndef VIDEOFRAMEDUMPER_H
#define VIDEOFRAMEDUMPER_H

#include <QAbstractVideoSurface>
#include <QMediaPlayer>

class VideoFrameDumper : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit VideoFrameDumper(QString path, QObject *parent = 0);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const
    {
        Q_UNUSED(handleType);

        return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_RGB24;
    }

    bool present(const QVideoFrame &frame);

    bool start(const QVideoSurfaceFormat & format) { return QAbstractVideoSurface::start(format); }
    void stop() { QAbstractVideoSurface::stop(); }

signals:
    void complete();

public slots:
    void mediaLoaded();
    void handleError();

private:
    QString m_outputPath;
    QMediaPlayer m_player;
    int m_counter;
    int m_requestedPosition;
};

#endif // VIDEOFRAMEDUMPER_H
