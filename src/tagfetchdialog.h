#ifndef TAGFETCHDIALOG_H
#define TAGFETCHDIALOG_H

#include "cheggitview.h"
#include "collection.h"

#include <QDialog>
#include <QListWidget>

class TagFetchDialog : public QDialog
{
Q_OBJECT
public:
    explicit TagFetchDialog(QString title, Collection *col, QWidget *parent = 0);

signals:
    void newTag(const QString &tag);

private slots:
    void moviesFetched(const QList<QPair<QString, QUrl> > &movies);
    void tagsFetched(const QStringList &tags);
    void save();
    void movieSelected(QModelIndex);

private:
    QListWidget *m_movieList;
    QListWidget *m_tagList;
    QList<QPair<QString, QUrl> > m_movies;
    CheggitView *m_cheggit;
    Collection *m_collection;
    QString m_videoName;
};

#endif // TAGFETCHDIALOG_H
