#include "coverloader.h"
#include <QMutexLocker>
#include "video.h"

CoverLoader::CoverLoader(QObject *parent) :
    QObject(parent)
{
}

void CoverLoader::loadVideo(Video *video)
{
    video->generateThumbnail();
}

