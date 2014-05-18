# Vimi

## Overview

Vimi is a cross-platform, tag based video manager written in Qt5.

### Usage

It assumes that each movie consists of a single directory, consisting of the
individual video files, an optional cover image and an optional file named
"tags.txt" containing tags for this move, one on each line.

When you add new tags, these will also be written to the "tags.txt" file in
the movie directory, as well as being written to the internal Vimi cache.

### Dependencies:

 * [Qt](http://qt-project.org/)

## Linux build

In a terminal, run the following in the directory containing the source:

    qmake && make

(Instructions for other platforms will follow.)

## TODO

info in control bar
automatic cumpilation
tag synonyms
import support
