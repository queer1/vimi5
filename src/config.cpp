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

Config::Config() : QObject()
{
    QSettings settings;

    m_collectionPath = settings.value("collectionPath", QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).first()).toString();
    m_actressPath = settings.value("actressPath", QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first() + "/actresses/").toString();
    m_coverSize = settings.value("coverSize", QSize(300, 200)).toSize();
    m_favouriteTags = settings.value("favouriteTags").toStringList();
    m_fullscreen = settings.value("fullscreen").toBool();
    m_actresses = settings.value("actresses").toStringList();
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
}

Config::~Config()
{
    save();
}

Config *Config::instance()
{
    static Config config;
    return &config;
}

void Config::save()
{
    QSettings settings;
    settings.setValue("collectionPath", m_collectionPath);
    settings.setValue("movieSuffixes", m_movieSuffixes);
    settings.setValue("coverSize", m_coverSize);
    settings.setValue("favouriteTags", m_favouriteTags);
    settings.setValue("fullscreen", m_fullscreen);
    settings.setValue("actresses", m_actresses);
    settings.setValue("actressPath", m_actressPath);
}
