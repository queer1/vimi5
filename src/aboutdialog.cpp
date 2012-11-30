/*
 * "About"-dialog
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

#include "aboutdialog.h"
#include "version.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    setLayout(new QVBoxLayout(this));
    m_logo = new QLabel();
    m_logo->setPixmap(QPixmap(":/images/splash.png"));

    m_header = new QLabel("<h2>Vimi v." + VERSION + "</h2>tag based video manager", this);
    m_text = new QLabel("Copyright 2012 cwk", this);
    m_header->setAlignment(Qt::AlignCenter);
    m_text->setAlignment(Qt::AlignCenter);

    m_button = new QPushButton("Close");

    layout()->addWidget(m_logo);
    layout()->addWidget(m_header);
    layout()->addWidget(m_text);
    layout()->addWidget(m_button);

    setWindowTitle("About...");

    connect(m_button, SIGNAL(clicked()), this, SLOT(accept()));
}
