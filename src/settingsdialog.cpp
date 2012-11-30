/*
 * Dialog for editing settings
 * Copyright (C) 2011-2012 cwk <coolwk@gmail.com>
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

#include "settingsdialog.h"
#include "config.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent),
    m_collectionPath(Config::collectionPath()),
    m_collectionPathBrowse("Browse..."),
    m_maxCoverSize(QString::number(Config::maxCoverSize())),
    m_dirExplorer(Config::dirExplorer()),
    m_moviePlayer(Config::moviePlayer()),
    m_saveButton("Save"),
    m_cancelButton("Cancel")
{
    QGridLayout *formLayout = new QGridLayout;

    QFormLayout *settingsLayout = new QFormLayout;
    formLayout->addLayout(settingsLayout, 0, 0, 1, 2);

    QHBoxLayout *collectionPathLayout = new QHBoxLayout;
    collectionPathLayout->addWidget(&m_collectionPath);
    collectionPathLayout->addWidget(&m_collectionPathBrowse);
    settingsLayout->addRow("Path to collection:", collectionPathLayout);

    settingsLayout->addRow("Maximum cover size (in pixels):", &m_maxCoverSize);
    m_maxCoverSize.setValidator(new QIntValidator);

    settingsLayout->addRow("File explorer (to open directories with):", &m_dirExplorer);
    settingsLayout->addRow("Video player:", &m_moviePlayer);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(&m_saveButton);
    buttonLayout->addWidget(&m_cancelButton);
    formLayout->addLayout(buttonLayout, 1, 0, 1, 1);

    connect(&m_collectionPathBrowse, SIGNAL(clicked()), SLOT(browseForCollection()));
    connect(&m_cancelButton, SIGNAL(clicked()), SLOT(accept()));
    connect(&m_saveButton, SIGNAL(clicked()), SLOT(save()));
    setLayout(formLayout);
}

void SettingsDialog::save()
{
    if (!m_collectionPath.text().isEmpty())
        Config::m_collectionPath = m_collectionPath.text();
    if (!m_maxCoverSize.text().isEmpty())
        Config::m_maxCoverSize = m_maxCoverSize.text().toInt();

    Config::m_dirExplorer = m_dirExplorer.text();
    Config::m_moviePlayer = m_moviePlayer.text();

    Config::save();
    accept();
}

void SettingsDialog::browseForCollection()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory Containing Collection"),
                                                    Config::collectionPath(),
                                                    QFileDialog::ShowDirsOnly);
    if (dir != "") {
        m_collectionPath.setText(dir);
    }
}
