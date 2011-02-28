#ifndef VIDEO_H
#define VIDEO_H
#include <QSet>
#include <QImage>
class QPixmap;
class QString;
class QStringList;
class QDataStream;

class Video
{
public:
    Video (QString path = "");

    QPixmap cover(int maxSize) const;
    QString path() const { return m_path; }
    QString name() const { return m_name; }
    QString tagList() const { return m_tagList; }

    QStringList files() const;
    const QSet<QString> &tags() const { return m_tags; }
    bool matchesTags (const QSet<QString> &tags) const;
    void addTag(QString tag);
    void removeTag(QString tag);
    void scanForCovers();
    static QImage quickScale(const QImage &source, int width, int height);

private:
    void writeTagCache();

    QString m_path;
    QString m_name;
    QString m_coverPath;
    QImage m_cover;
    QSet<QString> m_tags;
    QString m_tagList;
};

#endif // VIDEO_H
