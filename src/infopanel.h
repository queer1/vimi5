// Copyright 2009 cwk

#ifndef INFOPANEL_H
#define INFOPANEL_H

#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include "coverlabel.h"

class InfoPanel : public QGroupBox
{
    Q_OBJECT

public:
    explicit InfoPanel(QWidget *parent = 0);
    void setInfo(const QString &title);
    const QString &videoName() { return m_videoName; }

signals:
    void editTags();
    void createCovers();
    void selectedTag(const QString&);

private slots:
    void openVideo(const QString&);
    void openDirectory(const QString&);

private:
    void resizeEvent(QResizeEvent *event);

    QLabel *m_title;
    QLabel *m_tags;
    QLabel *m_files;
    QLabel *m_path;
    CoverLabel *m_cover;
    QString m_videoName;
};

#endif // INFOPANEL_H
