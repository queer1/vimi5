/*
 * Dialog to create covers from video files
 * Copyright (C) 2010-2012 cwk <coolwk@gmail.com>
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

#include "covermaker.h"
#include <QDebug>
#include <QPushButton>
#include "collection.h"
#include <QVBoxLayout>
#include <QImageWriter>
#include <QDir>
#include <QComboBox>
#include <QSlider>
#include <QScrollBar>
#include "videowidget.h"



CoverMaker::CoverMaker(QString videoName, QWidget *parent) : QDialog(parent),
    m_name(videoName)
{
    setWindowTitle("Create covers for " + videoName);
    setLayout(new QVBoxLayout(this));

    m_path = Collection::getPath(videoName);
    if (Collection::getFiles(videoName).size() > 1) {
        m_comboBox = new QComboBox(this);
        m_comboBox->addItems(Collection::getFiles(videoName));
        layout()->addWidget(m_comboBox);
        connect(m_comboBox, SIGNAL(activated(QString)), this, SLOT(selectVideo(QString)));
    }

    m_videoWidget = new VideoWidget(this, m_path + "/" +  Collection::getFiles(videoName).first());
    //m_videoWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_slider = new QScrollBar(this);
    m_slider->setMaximum(m_videoWidget->length());
    m_slider->setTracking(true);
    m_slider->setOrientation(Qt::Horizontal);
    m_slider->setValue(m_slider->maximum() / 2);

    QPushButton *saveButton = new QPushButton("Save");

    layout()->addWidget(saveButton);
    layout()->addWidget(m_slider);
    layout()->addWidget(m_videoWidget);

    connect(m_slider, SIGNAL(valueChanged(int)), m_videoWidget, SLOT(seek(int)));
    connect(saveButton, SIGNAL(clicked()), SLOT(saveFrame()));
}

void CoverMaker::selectVideo(QString file) {
    delete m_videoWidget;
    m_videoWidget = new VideoWidget(this, m_path + "/" + file);
    layout()->addWidget(m_videoWidget);
    m_slider->setMaximum(m_videoWidget->length());
    m_slider->setValue(0);
    connect(m_slider, SIGNAL(valueChanged(int)), m_videoWidget, SLOT(seek(int)));
}

void CoverMaker::saveFrame()
{
    QImage frame = m_videoWidget->getFrame();
    QDir dir(m_path);
    QString filename = "cover";
    if (dir.exists(filename + ".jpg")) {
        QString newName;
        int i = 0;
        do {
            newName = filename + QString::number(i);
            i++;
        } while (dir.exists(newName + ".jpg"));
        filename = newName;
    }
    QImageWriter writer (m_path + "/" + filename + ".jpg");
    if (writer.write(frame))
        emit coverCreated(m_name);
}
