/*
 * Dialog for editing tags for a given video
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

#ifndef TAGDIALOG_H
#define TAGDIALOG_H

#include "collection.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

class TagDialog : public QDialog
{
Q_OBJECT
public:
    explicit TagDialog(const QString &videoName, QWidget *parent = 0);

private slots:
    void addTag(QString tag = "");
    void removeTag();

private:
    void updateModel();


    QComboBox *m_tagEdit;
    QListWidget *m_tagView;
    QPushButton *m_addButton;
    QPushButton *m_removeButton;
    QPushButton *m_closeButton;
    QLabel *m_title;
    QString m_videoName;
    Collection *m_collection;
};

#endif // TAGDIALOG_H
