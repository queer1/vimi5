// Copyright 2009 cwk

#ifndef INFOPANEL_H
#define INFOPANEL_H

#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

class InfoPanel : public QGroupBox
{
    Q_OBJECT

public:
    explicit InfoPanel(QWidget *parent = 0);
    void setInfo(const QString &title);
    const QString &videoName() { return m_videoName; }

signals:
    void editTags();

private slots:
    void launchFile(const QString&);

private:
    QLabel *m_title;
    QLabel *m_tags;
    QLabel *m_files;
    QLabel *m_path;
    QLabel *m_cover;
    QPushButton *m_tagEditButton;
    QString m_videoName;

};

#endif // INFOPANEL_H
