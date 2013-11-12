#include "videoframedumper.h"
#include <QFileInfo>

VideoFrameDumper::VideoFrameDumper(QUrl path) :
    QAbstractVideoSurface(0),
    m_counter(0),
    m_requestedPosition(0),
    m_numberOfFrames(-1),
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
}

void VideoFrameDumper::createScreenshots(int numberOfFrames)
{
    m_numberOfFrames = numberOfFrames;
    m_player.play();
}

void VideoFrameDumper::createCover(qint64 position)
{
    m_requestedPosition = position;
    m_player.setPosition(position);
    m_player.play();
}

bool VideoFrameDumper::present(const QVideoFrame &f)
{
    QVideoFrame frame(f);

    if (qAbs(m_requestedPosition - frame.startTime()) > 10000) {
        qDebug() << "Got wrong frame!: " << m_requestedPosition << frame.startTime();
        m_wrongFrameCount++;

        if (m_wrongFrameCount < 3) {
            QMetaObject::invokeMethod(&m_player, "setPosition", Qt::QueuedConnection, Q_ARG(qint64, m_requestedPosition));
            return true;
        }
    } else if (frame.pixelFormat() == QVideoFrame::Format_RGB24) {
        frame.map(QAbstractVideoBuffer::ReadOnly);
        {
            QImage image(frame.bits(), frame.width(), frame.height(), QImage::Format_RGB888);
            QString filename;
            int height;
            if (m_numberOfFrames == -1) {
                filename = m_outputPath + "/.vimicover.jpg";
                height = 300;
            } else {
                filename = m_outputPath + "/.vimiframe_" + QString::number(frame.startTime()) + "_" + m_filename + ".jpg";
                height = 200;
                emit statusUpdated(QString("Screenshot progress %1%").arg(frame.startTime()*100 / m_player.duration()));
            }
            image.scaledToHeight(height, Qt::SmoothTransformation).save(filename);
            qDebug() << "saved" << filename;
        }
        frame.unmap();
    } else {
        qWarning() << "wrong pixel format:" << frame.pixelFormat();
    }


    // Generatic a single cover
    if (m_numberOfFrames == -1) {
        QMetaObject::invokeMethod(&m_player, "stop", Qt::QueuedConnection);
        emit coverCreated(m_outputPath);
        deleteLater();
        return true;
    }

    if (m_counter >= m_numberOfFrames) {
        QMetaObject::invokeMethod(&m_player, "stop", Qt::QueuedConnection);
        emit screenshotsCreated(m_outputPath);
        emit statusUpdated("Finished creating screenshots");
        deleteLater();
        return true;
    }

    m_wrongFrameCount = 0;
    m_requestedPosition = m_player.duration()*m_counter++ / m_numberOfFrames;
    QMetaObject::invokeMethod(&m_player, "setPosition", Qt::QueuedConnection, Q_ARG(qint64, m_requestedPosition));
    if (m_counter <= m_numberOfFrames)
        return true;
    else
        return false;
}

void VideoFrameDumper::mediaLoaded()
{
    if (m_player.mediaStatus() == QMediaPlayer::LoadedMedia) {
    }
    else if (m_player.mediaStatus() == QMediaPlayer::EndOfMedia) {
        m_player.stop();
        deleteLater();
    }
}

void VideoFrameDumper::handleError()
{
    qDebug() << Q_FUNC_INFO << m_player.errorString();
}
