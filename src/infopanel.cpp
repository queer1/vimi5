// Copyright 2009 cwk

#include "collection.h"
#include "infopanel.h"
#include <QDesktopServices>
#include <QDebug>
#include <QScrollArea>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QProcess>
#include <QFontMetrics>

InfoPanel::InfoPanel(QWidget *parent) :
    QGroupBox(parent)
{
    hide();

    //setTitle("Info");

    m_title = new QLabel("");
    m_title->setWordWrap(true);
    //m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_cover = new CoverLabel(this);
    //m_cover->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_tags = new QLabel("");
    m_files = new QLabel("");
    m_files->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_path = new QLabel("");
    QPushButton *tagEditButton = new QPushButton("&Edit tags...");
    QPushButton *createCoversButton = new QPushButton("&Create covers...");

    setLayout(new QVBoxLayout);
    layout()->addWidget(m_title);
    layout()->addWidget(m_cover);
    layout()->addWidget(m_tags);
    layout()->addWidget(m_files);
    layout()->addWidget(m_path);
    layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout()->addWidget(tagEditButton);
    layout()->addWidget(createCoversButton);

    connect(m_files, SIGNAL(linkActivated(QString)), SLOT(openVideo(QString)));
    connect(m_path, SIGNAL(linkActivated(QString)), SLOT(openDirectory(QString)));
    connect(m_tags, SIGNAL(linkActivated(QString)), SIGNAL(selectedTag(QString)));
    connect(tagEditButton, SIGNAL(clicked()), SIGNAL(editTags()));
    connect(createCoversButton, SIGNAL(clicked()), SIGNAL(createCovers()));
}

void InfoPanel::setInfo(const QString &title)
{
    if (title.isEmpty()) return;

    QFontMetrics m(font());

    QStringList tags = Collection::getTags(title);
    m_videoName = title;

    m_title->setText("<h3>" + title + "</h3>");

    if (!tags.empty()) {
        // List of tags
        QString tagHtml = "<b>Tags:</b><ul>";
        foreach (QString tag, tags) tagHtml += "<li><a href='" + tag + "'>" + tag + "</a></li>";
        tagHtml += "</ul>";
        m_tags->setText(tagHtml);
    } else {
        m_tags->clear();
    }

    // List of files
    QStringList files = Collection::getFiles(title);
    QString path = Collection::getPath(title);

    QString fileHtml = "<b>Files:</b><br/>";
    foreach (QString file, files)
        fileHtml += "<a href='" + QUrl::toPercentEncoding(path + "/" + file) + "'>" + m.elidedText(file, Qt::ElideRight, width()-20) + "</a><br/>";
    fileHtml += "";
    m_files->setText(fileHtml);

    m_path->setText("<br /> <a href='" + QUrl::toPercentEncoding(path) + "'>Click to open in file manager</a>");

    m_cover->setVideo(title);
    m_cover->repaint();

    show();
}

void InfoPanel::resizeEvent(QResizeEvent *)
{
    setInfo(m_videoName);
}

void InfoPanel::openDirectory(const QString &directory)
{
    if (Config::dirExplorer().isEmpty()) {
        QUrl url = QUrl::fromLocalFile(QUrl::fromPercentEncoding(directory.toLocal8Bit()));
        QDesktopServices::openUrl(url);//QUrl::fromPercentEncoding(file.toLocal8Bit()));
    } else {
        QUrl url = QUrl::fromLocalFile(QUrl::fromPercentEncoding(directory.toLocal8Bit()));
        QProcess::startDetached(Config::dirExplorer(), QStringList(url.toLocalFile()));
    }}

void InfoPanel::openVideo(const QString &file)
{
    if (Config::moviePlayer().isEmpty()) {
        qWarning() << "No video player configured, using platform default...";
        QUrl url = QUrl::fromLocalFile(QUrl::fromPercentEncoding(file.toLocal8Bit()));
        QDesktopServices::openUrl(url);//QUrl::fromPercentEncoding(file.toLocal8Bit()));
    } else {
        QUrl url = QUrl::fromLocalFile(QUrl::fromPercentEncoding(file.toLocal8Bit()));
        //qDebug() << Config::moviePlayer() << url.toLocalFile();
        QProcess::startDetached(Config::moviePlayer(), QStringList(url.toLocalFile()));
    }
}
