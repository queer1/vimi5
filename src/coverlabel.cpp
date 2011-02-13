#include "coverlabel.h"
#include "collection.h"
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

CoverLabel::CoverLabel(QWidget *parent) :
    QLabel(parent),
    m_dir(0)
{
    setCursor(Qt::PointingHandCursor);
    connect(&m_timer, SIGNAL(timeout()), SLOT(nextImage()));
}
CoverLabel::~CoverLabel()
{
    delete m_dir;
}

void CoverLabel::setVideo(const QString &video)
{
    m_videoName = video;
    setPixmap(Collection::getCover(m_videoName, 250));
    m_path = Collection::getPath(m_videoName);
}

void CoverLabel::leaveEvent(QEvent *)
{
    m_timer.stop();
    setPixmap(Collection::getCover(m_videoName, 250));
}

void CoverLabel::enterEvent(QEvent *)
{
    if (m_timer.isActive())
        return; // WTF

    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.png";
    delete m_dir;
    m_dir = new QDirIterator(m_path, nameFilters, QDir::Files, QDirIterator::NoIteratorFlags);
    m_timer.setInterval(0);
    m_timer.start();
}

void CoverLabel::nextImage()
{
    QImage image;
    // Sanity check sizes
    do {
        while (m_dir->fileInfo().size() > 1024*1024 && m_dir->hasNext()) // more than 1 MB
            m_dir->next();
        image = QImage(m_dir->next());
    } while ((image.height() > 1024 || image.width() > 1024) && m_dir->hasNext());

    if (!m_dir->hasNext()) { // restart
        m_timer.stop();
        enterEvent();
    }

    if (image.height() > height()) {
        float factor = (float)height() / image.height();
        image = Video::quickScale(image, image.width() * factor, image.height() * factor);
    }
    if (image.width() > width()) {
        float factor = (float)width() / image.width();
        image = Video::quickScale(image, image.width() * factor, image.height() * factor);
    }
    setPixmap(QPixmap::fromImage(image));
    m_timer.setInterval(1000);
}

void CoverLabel::mousePressEvent(QMouseEvent *)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_dir->filePath()));
}
