// Copyright 2009 cwk

#ifndef INFOPANEL_H
#define INFOPANEL_H

#include <QLabel>
#include <QWidget>

class InfoPanel : public QWidget
{
    Q_OBJECT

public:
    explicit InfoPanel(QWidget *parent = 0);
    void setInfo(const QString &title, const QStringList &tags, const QStringList &files, const QString &path);

private slots:
    void launchFile(const QString&);

private:
    QLabel *m_title;
    QLabel *m_tags;
    QLabel *m_files;
};

#endif // INFOPANEL_H
