#ifndef VIDEO_H
#define VIDEO_H
#include <QStringList>
#include <QImage>
#include <QPixmap>
#include <QObject>
class QString;
class QDataStream;

class Video : public QObject
{
    Q_OBJECT
public:
    Video (QObject *parent, QString path = "");
    Video (QObject *parent, QString path, QString tags, QString coverPath);

    QPixmap cover(int maxSize);
    //const QImage cover();
    QString coverPath() const { return m_coverPath; }
    QString path() const { return m_path; }
    QString name() const { return m_name; }
    QString tagList() const { return m_tagList; }

    QStringList files() const;
    const QStringList &tags() const { return m_tags; }
    void addTag(QString tag);
    void removeTag(QString tag);
    void scanForCovers();
    static QImage quickScale(const QImage &source, int width, int height);
    void loadCover();

signals:
    void coverLoaded(const QString& name);

private:
    void writeTagCache();

    QString m_path;
    QString m_name;
    QString m_coverPath;
    QImage m_cover;
    QStringList m_tags;
    QString m_tagList;
    QPixmap m_coverIconCache;

    static QPixmap *m_defaultCover;
};

#endif // VIDEO_H
