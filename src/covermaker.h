#ifndef COVERMAKER_H
#define COVERMAKER_H

#include <QDialog>

class QComboBox;
class VideoWidget;
class QSlider;

class CoverMaker : public QDialog
{
    Q_OBJECT
public:
    CoverMaker(QString videoName, QWidget *parent = 0);

signals:
    void coverCreated(QString name);

private slots:
    void selectVideo(QString);
    void saveFrame();

private:
    QComboBox *m_comboBox;
    VideoWidget *m_videoWidget;
    QSlider *m_slider;
    QString m_path;
    QString m_name;
};


#endif//COVERMAKER_H
