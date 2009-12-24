// Copyright 2009 cwk

#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QStringList>

class Config
{
public:
    Config();

    static QString collectionPath;
    static QStringList movieSuffixes;
};

#endif // CONFIG_H
