#ifndef VIDEOFRAMEDUMPER_H
#define VIDEOFRAMEDUMPER_H

#include <QAbstractVideoSurface>
#include <QMediaPlayer>

class VideoFrameDumper : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit VideoFrameDumper(QUrl path);
    void createScreenshots(int numberOfFrames);
    void createCover(qint64 position);
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
    void screenshotsCreated(QString path);
    void coverCreated(QString path);
    void statusUpdated(QString status);

public slots:
    void mediaLoaded();
    void handleError();

private:
    QString m_outputPath;
    QString m_filename;
    QMediaPlayer m_player;
    int m_counter;
    qint64 m_requestedPosition;
    int m_numberOfFrames;
    int m_wrongFrameCount;
};

#endif // VIDEOFRAMEDUMPER_H
