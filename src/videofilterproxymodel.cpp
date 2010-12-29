// Copyright 2009 cwk

#include "videofilterproxymodel.h"

#include <QDebug>

VideoFilterProxyModel::VideoFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool VideoFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QString name = sourceModel()->data(sourceModel()->index(sourceRow, 0, sourceParent)).toString();
    QString tags = sourceModel()->data(sourceModel()->index(sourceRow, 1, sourceParent)).toString();

    if (!m_filter.isEmpty() && !name.contains(m_filter, Qt::CaseInsensitive) && !tags.contains(m_filter, Qt::CaseInsensitive))
        return false;

    foreach (QString tag, m_tagList) {
        if (!tags.contains(tag, Qt::CaseInsensitive))
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
