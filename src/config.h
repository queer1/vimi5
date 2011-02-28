// Copyright 2009 cwk

#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QStringList>

class Config
{
public:
    static QString collectionPath();
    static QStringList movieSuffixes();
    static int maxCoverSize();

    static void setCollectionPath(QString path);
private:
    static void load();
    static void save();

    static QString m_collectionPath;
    static QStringList m_movieSuffixes;
    static int m_maxCoverSize;
    static bool m_loaded;
};

#endif // CONFIG_H
