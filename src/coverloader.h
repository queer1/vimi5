#ifndef COVERLOADER_H
#define COVERLOADER_H

#include <QObject>
#include <QList>
#include <QMutex>
#include <QWaitCondition>

class Video;
class CoverLoader : public QObject
{
    Q_OBJECT
public:
    explicit CoverLoader(QObject *parent = 0);
    bool running;

public slots:
    void loadVideo(Video *video);
};

#endif // COVERLOADER_H
