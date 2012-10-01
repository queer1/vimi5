// Copyright 2009 cwk

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
