/*
 * Filtering proxy model that filters on all columns
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

#include "videofilterproxymodel.h"

#include <QDebug>

VideoFilterProxyModel::VideoFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool VideoFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_filter.isEmpty() && m_tagList.isEmpty())
        return true;

    const QString &name = sourceModel()->data(sourceModel()->index(sourceRow, 1, sourceParent)).toString();
    const QString &tags = sourceModel()->data(sourceModel()->index(sourceRow, 2, sourceParent)).toString();

    if (!m_filter.isEmpty() && !name.contains(m_filter, Qt::CaseInsensitive) && !tags.contains(m_filter, Qt::CaseInsensitive))
        return false;

    if (m_tagList.isEmpty())
        return true;

    const QStringList &tagList = sourceModel()->data(sourceModel()->index(sourceRow, 3, sourceParent)).toStringList();
    foreach (const QString &tag, m_tagList) {
        if (!tagList.contains(tag, Qt::CaseInsensitive))
            return false;
    }

    return true; // Also true if the list of tags is empty
}

void VideoFilterProxyModel::addTag(const QString &tag)
{
    if (m_tagList.contains(tag))
        return;

    m_tagList.append(tag);
    invalidateFilter();
}

void VideoFilterProxyModel::removeTag(const QString &tag)
{
    if (m_tagList.removeAll(tag) > 0)
        invalidateFilter();
}

void VideoFilterProxyModel::setFilterFixedString(const QString &filter)
{
    m_filter = filter;
    invalidateFilter();
}

const QStringList &VideoFilterProxyModel::activeTagFilters()
{
    return m_tagList;
}

void VideoFilterProxyModel::clearTags()
{
    m_tagList.clear();
    invalidateFilter();
}
