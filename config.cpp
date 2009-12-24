// Copyright 2009 cwk

#include "config.h"

QString Config::collectionPath;
QStringList Config::movieSuffixes;

Config::Config()
{
    collectionPath = "/home/w00t/test/";
    movieSuffixes.clear();
    movieSuffixes << "*.mpg"
                  << "*.wmv"
                  << "*.mkv"
                  << "*.avi"
                  << "*.mp4";
}
