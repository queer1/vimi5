/*
 * Panel widget that shows information about the selected video
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

#ifndef INFOPANEL_H
#define INFOPANEL_H

#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include "coverlabel.h"

class InfoPanel : public QGroupBox
{
    Q_OBJECT

public:
    explicit InfoPanel(QWidget *parent = 0);
    void setInfo(const QString &title);
    const QString &videoName() { return m_videoName; }

signals:
    void editTags();
    void createCovers();
    void selectedTag(const QString&);

private slots:
    void openVideo(const QString&);
    void openDirectory(const QString&);

private:
    void resizeEvent(QResizeEvent *event);

    QLabel *m_title;
    QLabel *m_tags;
    QLabel *m_files;
    QLabel *m_path;
    CoverLabel *m_cover;
    QString m_videoName;
};

#endif // INFOPANEL_H
