#ifndef VIDEOFRAMEDUMPER_H
#define VIDEOFRAMEDUMPER_H

#include <QObject>
#include <QThread>
#include <QUrl>

class VideoFrameDumper : public QObject
{
    Q_OBJECT
public:
    explicit VideoFrameDumper(QUrl path);

    void createSnapshots(int num);

signals:
    void screenshotsCreated(QString path);
    void coverCreated(QString path);
    void statusUpdated(QString status);


private:
    QThread m_thread;
    QString m_outputPath;
    QByteArray m_filename;
};

#endif // VIDEOFRAMEDUMPER_H
