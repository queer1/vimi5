// Copyright 2009 cwk

#include "config.h"
#include <QDesktopServices>
#include <QSettings>

QString Config::m_collectionPath;
QStringList Config::m_movieSuffixes;
int Config::m_maxCoverSize;
bool Config::m_loaded = false;

void Config::load()
{
    QSettings settings;

    m_collectionPath = settings.value("collectionPath", QDesktopServices::storageLocation(QDesktopServices::MoviesLocation)).toString();

    m_maxCoverSize = settings.value("maxCoverSize", 128).toInt();

    QStringList defaultSuffixes;
    defaultSuffixes << "*.mpg"
                    << "*.wmv"
                    << "*.mkv"
                    << "*.avi"
                    << "*.mp4";
    QStringList settingSuffixes = settings.value("movieSuffixes", defaultSuffixes).toStringList();
    if (settingSuffixes.length() > 0)
        m_movieSuffixes = settingSuffixes;
    else
        m_movieSuffixes = defaultSuffixes;

    save(); // Save default config
}

void Config::save()
{
    QSettings settings;
    settings.setValue("collectionPath", m_collectionPath);
    settings.setValue("movieSuffixes", m_movieSuffixes);
    settings.setValue("maxCoverSize", m_maxCoverSize);
}

QString Config::collectionPath()
{
    if (!m_loaded)
        load();
    return m_collectionPath;
}

QStringList Config::movieSuffixes()
{
    if (!m_loaded)
        load();
    return m_movieSuffixes;
}

int Config::maxCoverSize()
{
    if (!m_loaded)
        load();
    return m_maxCoverSize;
}

void Config::setCollectionPath(QString path)
{
    if (!m_loaded)
        load();

    m_collectionPath = path;
    save();
}
