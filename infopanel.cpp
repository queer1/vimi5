// Copyright 2009 cwk

#include "infopanel.h"
#include <QDesktopServices>
#include <QUrl>
#include <QVBoxLayout>

InfoPanel::InfoPanel(QWidget *parent) :
    QWidget(parent)
{
    m_title = new QLabel("");
    m_tags = new QLabel("");
    m_files = new QLabel("");

    setLayout(new QVBoxLayout);
    layout()->addWidget(m_title);
    layout()->addWidget(m_tags);
    layout()->addWidget(m_files);

    connect(m_files, SIGNAL(linkActivated(QString)), SLOT(launchFile(QString)));
}

void InfoPanel::setInfo(const QString &title, const QStringList &tags, const QStringList &files, const QString &path)
{
    m_title->setText("<h1>" + title + "</h1>");

    // List of tags
    QString tagHtml = "<b>Tags:</b><ul>";
    foreach (QString tag, tags) tagHtml += "<li>" + tag + "</li>";
    tagHtml += "</ul>";
    m_tags->setText(tagHtml);

    // List of files
    QString fileHtml = "<b>Files:</b><ul>";
    foreach (QString file, files)
        fileHtml += "<li><a href='" + path + "/" + file + "'>" + file + "</a></li>";
    fileHtml += "</ul>";
    m_files->setText(fileHtml);
}

void InfoPanel::launchFile(const QString &file)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(file));
}
