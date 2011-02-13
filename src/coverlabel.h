#ifndef COVERLABEL_H
#define COVERLABEL_H

#include <QLabel>
#include <QDirIterator>
#include <QTimer>

class CoverLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CoverLabel(QWidget *parent = 0);
    ~CoverLabel();
    void setVideo(const QString &video);

private slots:
    void nextImage();

private:
    void leaveEvent(QEvent *);
    void enterEvent(QEvent * = 0);
    void mousePressEvent(QMouseEvent *ev);

    QString m_videoName;
    QString m_path;
    QDirIterator *m_dir;
    QTimer m_timer;
};

#endif // COVERLABEL_H
