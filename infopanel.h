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
    void setInfo(const QString &title, const QStringList &tags, const QStringList &files, const QString &path);

signals:
    void editTags();

private slots:
    void launchFile(const QString&);

private:
    QLabel *m_title;
    QLabel *m_tags;
    QLabel *m_files;
    QPushButton *m_tagEditButton;
};

#endif // INFOPANEL_H
