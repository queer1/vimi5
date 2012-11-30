/*
 * Dialog for replacing all tags with another tag in the collection
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

#include "tagreplacementdialog.h"
#include "collection.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QCompleter>
#include <QPushButton>

TagReplacementDialog::TagReplacementDialog(QWidget *parent) :
    QDialog(parent)
{
    QGridLayout *mainLayout = new QGridLayout;

    m_existingTag = new QComboBox(this);
    m_existingTag->setEditable(false);
    m_existingTag->addItems(Collection::getTags());


    m_newTag = new QComboBox(this);
    m_newTag->setEditable(true);
    m_newTag->setCompleter(new QCompleter(Collection::getTags(), this));
    m_newTag->addItems(Collection::getTags());

    QPushButton *doChangeButton = new QPushButton("Replace all tags");
    connect(doChangeButton, SIGNAL(clicked()), SLOT(doChange()));

    setLayout(mainLayout);
    mainLayout->addWidget(m_existingTag, 0, 0);
    mainLayout->addWidget(m_newTag, 0, 1);
    mainLayout->addWidget(doChangeButton, 1, 1);
}

void TagReplacementDialog::doChange()
{
    Collection::replaceTag(m_existingTag->currentText(), m_newTag->currentText());
}
