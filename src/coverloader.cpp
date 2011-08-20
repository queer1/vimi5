#include "coverloader.h"
#include <QMutexLocker>
#include "video.h"
#include <QDebug>

CoverLoader::CoverLoader(QObject *parent) :
    QObject(parent)
{
    running = true;
}

void CoverLoader::loadVideo(Video *video)
{
    if (!running) return;
    video->generateThumbnail();
}

