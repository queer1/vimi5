#ifndef VIDEO_H
#define VIDEO_H
#include <QStringList>
#include <QImage>
#include <QPixmap>
#include <QObject>

class QString;
class QDataStream;
class Collection;

class Video : public QObject
{
    Q_OBJECT
public:
    Video (Collection *parent, QString path, QString tags, QString coverPath);

    QPixmap cover(int maxSize);
    const QPixmap &thumbnail();
    //const QImage cover();
    QString coverPath() const { return m_coverPath; }
    QString path() const { return m_path; }
    QString name() const { return m_name; }
    QString tagList() const { return m_tagList; }

    QStringList files() const;
    const QStringList &tags() const { return m_tags; }
    void addTag(QString tag);
    void removeTag(QString tag);
    static QString scanForCovers(QString path);
    void scanForCovers() { m_coverPath = scanForCovers(m_path); }
    static QImage quickScale(const QImage &source, int width, int height);
    static Video *makeVideo (Collection *parent, QString path = "");
    void generateThumbnail();

signals:
    void coverLoaded(const QString& name);
    void needToLoadCover(Video *me);

private:
    void writeTagCache();

    QString m_path;
    QString m_name;
    QString m_coverPath;
    QImage *m_cover;
    QStringList m_tags;
    QString m_tagList;
    QPixmap m_thumbnail;
    Collection *m_collection;

    static QPixmap *m_defaultCover;
    static QPixmap *m_defaultThumbnail;
};

#endif // VIDEO_H
