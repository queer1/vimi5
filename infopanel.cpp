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
    m_tagEditButton = new QPushButton("Edit tags...");

    setLayout(new QVBoxLayout);
    layout()->addWidget(m_title);
    layout()->addWidget(m_cover);
    layout()->addWidget(m_tags);
    layout()->addWidget(m_files);
    layout()->addWidget(m_path);
    layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout()->addWidget(m_tagEditButton);

    connect(m_files, SIGNAL(linkActivated(QString)), SLOT(launchFile(QString)));
    connect(m_path, SIGNAL(linkActivated(QString)), SLOT(launchFile(QString)));
    connect(m_tagEditButton, SIGNAL(clicked()), SIGNAL(editTags()));
}

void InfoPanel::setInfo(const QString &title)
{
    QStringList tags = Collection::getTags(title);
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
    QPixmap cover = Collection::getCover(title);
    if (qMax(cover.height(), cover.width()) > 250) {
        float factor = 250.0f / qMax(cover.height(), cover.width());
        cover = cover.scaled(cover.height() * factor, cover.width() * factor, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    m_cover->setPixmap(cover);
    m_cover->repaint();

    show();
}

void InfoPanel::launchFile(const QString &file)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(file));
}

