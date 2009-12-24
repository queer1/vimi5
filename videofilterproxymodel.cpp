// Copyright 2009 cwk

#include "videofilterproxymodel.h"

VideoFilterProxyModel::VideoFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool VideoFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 1, sourceParent);

    QString videoTags = sourceModel()->data(index).toString();

    foreach (QString tag, m_tagList) {
        if (!videoTags.contains(tag))
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
