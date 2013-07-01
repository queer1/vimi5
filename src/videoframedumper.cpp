#include "videoframedumper.h"
#include <QFileInfo>

VideoFrameDumper::VideoFrameDumper(QString path, QObject *parent) :
    QAbstractVideoSurface(parent),
    m_counter(0),
    m_requestedPosition(0)
{
    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        qWarning() << "can't open file" << path;
    }

    m_outputPath = fileInfo.canonicalPath();

    m_player.setVideoOutput(this);
    m_player.setMedia(QUrl::fromLocalFile(path));
    connect(&m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), SLOT(mediaLoaded()));
    connect(&m_player, SIGNAL(error(QMediaPlayer::Error)), SLOT(handleError()));
    m_player.play();
}

bool VideoFrameDumper::present(const QVideoFrame &f)
{
    QVideoFrame frame(f);
    qDebug() << m_requestedPosition << frame.startTime();
    if (frame.pixelFormat() == QVideoFrame::Format_RGB24) {
        frame.map(QAbstractVideoBuffer::ReadOnly);
        {
            QImage image(frame.bits(), frame.width(), frame.height(), QImage::Format_RGB888);
            image.save(m_outputPath + "/vimiframe_" + QString::number(m_requestedPosition) + ".png");
            qDebug() << "saved to" << m_outputPath + "/vimiframe_" + QString::number(m_requestedPosition) + ".png";
        }
        frame.unmap();
    } else {
        qWarning() << "wrong pixel format:" << frame.pixelFormat();
    }

    m_requestedPosition = m_player.duration()*m_counter++ / 10;
    //m_player.play();
    QMetaObject::invokeMethod(&m_player, "setPosition", Qt::QueuedConnection, Q_ARG(qint64, m_requestedPosition));
    if (m_counter <= 10)
        return true;
    else
        return false;
}

void VideoFrameDumper::mediaLoaded()
{
    qDebug() << m_player.mediaStatus();
    if (m_player.mediaStatus() == QMediaPlayer::LoadedMedia) {
    }
    else if (m_player.mediaStatus() == QMediaPlayer::EndOfMedia) {
        m_player.stop();
        emit complete();
    }

}

void VideoFrameDumper::handleError()
{
    qDebug() << m_player.errorString();
}
