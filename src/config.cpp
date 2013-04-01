/*
 * Holds application configuration
 * Copyright (C) 2009-2012 cwk <coolwk@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <QStandardPaths>
#include <QSettings>
#include <QDebug>

QString Config::m_collectionPath;
QString Config::m_dirExplorer;
QString Config::m_moviePlayer;
QStringList Config::m_movieSuffixes;
QStringList Config::m_favouriteTags;
int Config::m_maxCoverSize;
bool Config::m_loaded = false;

void Config::load()
{
    QSettings settings;

    m_collectionPath = settings.value("collectionPath", QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).first()).toString();
    m_maxCoverSize = settings.value("maxCoverSize", 128).toInt();
    m_dirExplorer = settings.value("dirExplorer", "").toString();
    m_moviePlayer = settings.value("moviePlayer", "").toString();
    m_favouriteTags = settings.value("favouriteTags").toStringList();


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
    m_loaded = true;
}

void Config::save()
{
    QSettings settings;
    settings.setValue("collectionPath", m_collectionPath);
    settings.setValue("movieSuffixes", m_movieSuffixes);
    settings.setValue("maxCoverSize", m_maxCoverSize);
    settings.setValue("moviePlayer", m_moviePlayer);
    settings.setValue("dirExplorer", m_dirExplorer);
    settings.setValue("favouriteTags", m_favouriteTags);


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

QString Config::dirExplorer()
{
    if (!m_loaded)
        load();

    return m_dirExplorer;
}


QString Config::moviePlayer()
{
    if (!m_loaded)
        load();

    return m_moviePlayer;
}

const QStringList &Config::favouriteTags()
{
    if (!m_loaded)
        load();

    return m_favouriteTags;
}

void Config::addFavouriteTag(const QString &tag)
{
    if (!m_loaded)
        load();

    m_favouriteTags.append(tag);
    save();
}

void Config::removeFavouriteTag(const QString &tag)
{
    if (!m_loaded)
        load();

    m_favouriteTags.removeAll(tag);
    save();
}

void Config::setCollectionPath(QString path)
{
    if (!m_loaded)
        load();

    m_collectionPath = path;
    save();
}

