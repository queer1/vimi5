// Copyright 2009 cwk

#include "collection.h"
#include "infopanel.h"
#include <QDesktopServices>
#include <QDebug>
#include <QScrollArea>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

InfoPanel::InfoPanel(QWidget *parent) :
    QGroupBox(parent)
{
    hide();

    setTitle("Info");

    m_title = new QLabel("");
    m_cover = new QLabel();

    m_tags = new QLabel("");
    m_files = new QLabel("");
    m_path = new QLabel("");
    QPushButton *tagEditButton = new QPushButton("&Edit tags...");
    QPushButton *tagFetchButton = new QPushButton("&Fetch tags...");

    setLayout(new QVBoxLayout);
    layout()->addWidget(m_title);
    layout()->addWidget(m_cover);
    layout()->addWidget(m_tags);
    layout()->addWidget(m_files);
    layout()->addWidget(m_path);
    layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout()->addWidget(tagEditButton);
    layout()->addWidget(tagFetchButton);

    connect(m_files, SIGNAL(linkActivated(QString)), SLOT(launchFile(QString)));
    connect(m_path, SIGNAL(linkActivated(QString)), SLOT(launchFile(QString)));
    connect(tagEditButton, SIGNAL(clicked()), SIGNAL(editTags()));
    connect(tagFetchButton, SIGNAL(clicked()), SIGNAL(fetchTags()));
}

void InfoPanel::setInfo(const QString &title)
{
    QSet<QString> tags = Collection::getTags(title);
    m_videoName = title;

    m_title->setText("<h3>" + title + "</h3>");

    // List of tags
    QString tagHtml = "<b>Tags:</b><ul>";
    foreach (QString tag, tags) tagHtml += "<li>" + tag + "</li>";
    tagHtml += "</ul>";
    m_tags->setText(tagHtml);

    // List of files
    QStringList files = Collection::getFiles(title);
    QString path = Collection::getPath(title);

    QString fileHtml = "<b>Files:</b><ul>";
    foreach (QString file, files)
        fileHtml += "<li><a href='" + path + "/" + file + "'>" + file + "</a></li>";
    fileHtml += "</ul>";
    m_files->setText(fileHtml);

    m_path->setText("<br /> <a href='" + path + "'>Click to open in file manager</a>");

    m_cover->setPixmap(QPixmap());
    QPixmap cover = Collection::getCover(title, 250);
    m_cover->setPixmap(cover);
    m_cover->repaint();

    show();
}

void InfoPanel::launchFile(const QString &file)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(file));
}

