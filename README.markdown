# Vimi

## Overview

Vimi is a cross-platform, tag based video manager written in Qt4.

### Usage

It assumes that each movie consists of a single directory, consisting of the
individual video files, an optional cover image and an optional file named
"tags.txt" containing tags for this move, one on each line.

When you add new tags, these will also be written to the "tags.txt" file in
the movie directory, as well as being written to the internal Vimi cache.

### Dependencies:

 * [Qt](http://qt.nokia.com/)
 * [ffmpeg](http://www.ffmpeg.org/)
    Windows builds of ffmpeg are available from http://ffmpeg.zeranoe.com/

## Develop

The easiest is to use [QtCreator](http://qt.nokia.com/products/developer-tools).

After installing and starting QtCreator, go to the File menu, select "New File
or Project", scroll down to "Git Repository Clone" and hit OK.

In "Clone URL" enter `git://github.com/cwk/Vimi.git`, select where you want it
to store it on your disk, and hit "Next" and "Finish".

You should now have a checkout of the code, and be ready to run it, by clicking
on the big green "play" button.


## Linux build

In a terminal, run the following in the directory containing the source:

    qmake && make

(Instructions for other platforms will follow.)

