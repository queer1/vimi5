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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);

signals:

private slots:
    void browseForCollection();
    void save();

private:
    QLineEdit m_collectionPath;
    QPushButton m_collectionPathBrowse;

    //QListWidget m_movieSuffixList;

    QLineEdit m_maxCoverSize;
    QLineEdit m_dirExplorer;
    QLineEdit m_moviePlayer;

    QPushButton m_saveButton;
    QPushButton m_cancelButton;
};

#endif // SETTINGSDIALOG_H
