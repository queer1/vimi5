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

#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QStringList>
class SettingsDialog;
class Config
{
public:
    static QString collectionPath();
    static QStringList movieSuffixes();
    static int maxCoverSize();
    static QString dirExplorer();
    static QString moviePlayer();
    static const QStringList &favouriteTags();

    static void addFavouriteTag(const QString&);
    static void removeFavouriteTag(const QString&);
    static void setCollectionPath(QString path);

    friend class SettingsDialog;
private:
    static void load();
    static void save();

    static bool m_loaded;

    static QString m_collectionPath;
    static QStringList m_movieSuffixes;
    static QStringList m_favouriteTags;
    static int m_maxCoverSize;
    static QString m_dirExplorer;
    static QString m_moviePlayer;
};

#endif // CONFIG_H
