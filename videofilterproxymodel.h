// Copyright 2009 cwk

#ifndef VIDEOFILTERPROXYMODEL_H
#define VIDEOFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>

class VideoFilterProxyModel : public QSortFilterProxyModel
{
public:
    VideoFilterProxyModel(QObject *parent=0);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

    void addTag(const QString &tag);
    void removeTag(const QString &tag);

private:
    QStringList m_tagList;
};

#endif // VIDEOFILTERPROXYMODEL_H
