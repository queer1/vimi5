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

#include <QObject>
#include <QString>
#include <QStringList>
#include <QSize>
#include <QDebug>
#include <QUrl>

class Config : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString collectionPath READ collectionPath WRITE setCollectionPath NOTIFY collectionPathChanged)
    Q_PROPERTY(QStringList movieSuffixes READ movieSuffixes WRITE setMovieSuffixes NOTIFY movieSuffixesChanged)
    Q_PROPERTY(int coverSize READ coverSize WRITE setCoverSize NOTIFY coverSizeChanged)
    Q_PROPERTY(QStringList favouriteTags READ favouriteTags WRITE setFavouriteTags NOTIFY favouriteTagsChanged)
    Q_PROPERTY(bool fullscreen READ fullscreen WRITE setFullscreen NOTIFY fullscreenChanged)
    Q_PROPERTY(QString actressPath READ actressesPath WRITE setActressesPath NOTIFY actressPathChanged)
    Q_PROPERTY(bool configShow READ configShow WRITE setConfigShow NOTIFY configShowChanged)
    Q_PROPERTY(bool starletsShow READ starletsShow WRITE setStarletsShow NOTIFY starletsShowChanged)


public:
    ~Config();

    QString collectionPath() { return m_collectionPath; }
    //void setCollectionPath(const QString &path) { qDebug() << path; m_collectionPath = path; save(); emit collectionPathChanged(); }
    void setCollectionPath(const QUrl &url) { qDebug() << "url" << url; m_collectionPath = url.toLocalFile(); save(); emit collectionPathChanged(); }


    const QStringList &movieSuffixes() { return m_movieSuffixes; }
    void setMovieSuffixes(const QStringList &suffixes) { m_movieSuffixes = suffixes; save(); emit movieSuffixesChanged(); }

    int coverSize() { return m_coverSize; }
    void setCoverSize(const int &size) { m_coverSize = size; save(); emit coverSizeChanged(); }

    const QStringList &favouriteTags() { return m_favouriteTags; }
    void setFavouriteTags(const QStringList &tags) { m_favouriteTags = tags; save(); emit favouriteTagsChanged(); }

    void addFavouriteTag(const QString &tag) { m_favouriteTags.append(tag); save(); emit favouriteTagsChanged(); }
    void removeFavouriteTag(const QString &tag) { m_favouriteTags.removeAll(tag); save(); emit favouriteTagsChanged(); }

    bool fullscreen() { return m_fullscreen; }
    void setFullscreen(bool fullscreen) { m_fullscreen = fullscreen; save(); emit fullscreenChanged(fullscreen); }

    const QString &actressesPath() { return m_actressPath; }
    void setActressesPath(const QString actressPath) { m_actressPath = actressPath; emit actressPathChanged(); }

    bool configShow() const { return m_configShow; }
    void setConfigShow(bool show) { m_configShow = show; emit configShowChanged(); }

    bool starletsShow() const { return m_starletsShow; }
    void setStarletsShow(bool show) { m_starletsShow = show; emit starletsShowChanged(); }

    static Config *instance();

signals:
    void collectionPathChanged();
    void movieSuffixesChanged();
    void coverSizeChanged();
    void favouriteTagsChanged();
    void fullscreenChanged(bool);
    void actressPathChanged();
    void configShowChanged();
    void starletsShowChanged();

private:
    Config();
    void save();

    QString m_collectionPath;
    QStringList m_movieSuffixes;
    QStringList m_favouriteTags;
    int m_coverSize;
    bool m_fullscreen;
    QString m_actressPath;
    bool m_configShow;
    bool m_starletsShow;
};

#endif // CONFIG_H
