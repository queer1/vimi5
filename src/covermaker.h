#ifndef COVERMAKER_H
#define COVERMAKER_H

#include <QDialog>
#include <QSlider>
#include <QComboBox>
#include "videowidget.h"
#include "video.h"

class CoverMaker : public QDialog
{
    Q_OBJECT
public:
    CoverMaker(QString videoName, QWidget *parent = 0);

private slots:
    void selectVideo(QString);
    void saveFrame();

private:
    QComboBox *m_comboBox;
    VideoWidget *m_videoWidget;
    QSlider *m_slider;
    QString m_path;
};


#endif//COVERMAKER_H
