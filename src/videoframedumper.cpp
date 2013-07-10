#include "videoframedumper.h"
#include <QFileInfo>

VideoFrameDumper::VideoFrameDumper(QUrl path, int numberOfFrames, QObject *parent) :
    QAbstractVideoSurface(parent),
    m_counter(0),
    m_requestedPosition(0),
    m_numberOfFrames(numberOfFrames),
    m_wrongFrameCount(0)
{
    QFileInfo fileInfo(path.toLocalFile());
    if (!fileInfo.exists()) {
        qWarning() << "can't open file" << path;
    }

    m_outputPath = fileInfo.canonicalPath();
    m_filename = fileInfo.fileName();

    m_player.setVideoOutput(this);
    m_player.setMedia(path);
    connect(&m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), SLOT(mediaLoaded()));
    connect(&m_player, SIGNAL(error(QMediaPlayer::Error)), SLOT(handleError()));
    m_player.play();
}

bool VideoFrameDumper::present(const QVideoFrame &f)
{
    QVideoFrame frame(f);

    if (qAbs(m_requestedPosition - frame.startTime()) > 100) {
        qDebug() << "Got wrong frame!: " << m_requestedPosition << frame.startTime();
        m_wrongFrameCount++;

        if (m_wrongFrameCount < 3) {
            QMetaObject::invokeMethod(&m_player, "setPosition", Qt::QueuedConnection, Q_ARG(qint64, m_requestedPosition));
            return true;
        }
    } else if (frame.pixelFormat() == QVideoFrame::Format_RGB24) {
        qDebug() << (frame.startTime()*100 / m_player.duration()) << "%";
        frame.map(QAbstractVideoBuffer::ReadOnly);
        {
            QImage image(frame.bits(), frame.width(), frame.height(), QImage::Format_RGB888);
            image.save(m_outputPath + "/vimiframe_" + QString::number(frame.startTime()) + "_" + m_filename + ".png");
            qDebug() << "saved to" << m_outputPath + "/vimiframe_" + QString::number(frame.startTime()) + "_" + m_filename + ".png";
        }
        frame.unmap();
    } else {
        qWarning() << "wrong pixel format:" << frame.pixelFormat();
    }

    m_wrongFrameCount = 0;
    m_requestedPosition = m_player.duration()*m_counter++ / m_numberOfFrames;
    //m_player.play();
    QMetaObject::invokeMethod(&m_player, "setPosition", Qt::QueuedConnection, Q_ARG(qint64, m_requestedPosition));
    if (m_counter <= m_numberOfFrames)
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
        emit complete(m_outputPath);
        deleteLater();
    }

}

void VideoFrameDumper::handleError()
{
    qDebug() << m_player.errorString();
}
