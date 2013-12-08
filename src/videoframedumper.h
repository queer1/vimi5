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

public slots:
    void createSnapshots(int num=100);

signals:
    void screenshotsCreated(QString path);
    void coverCreated(QString path);
    void statusUpdated(QString status);


private:
    QThread m_thread;
    QString m_outputPath;
    QString m_filename;
    QByteArray m_outputFile;
};

#endif // VIDEOFRAMEDUMPER_H
