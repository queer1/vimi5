#include "tagfetchdialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QMetaMethod>
#include <QMessageBox>
#include <QUrl>

TagFetchDialog::TagFetchDialog(QString movieTitle, QWidget *parent) : QDialog(parent),
    m_movieList(new QListWidget),
    m_tagList(new QListWidget),
    m_cheggit(CheggitView::instance()),
    m_videoName(movieTitle)
{
    setWindowTitle("Searching for tags for: " + movieTitle);

    QGridLayout *layout = new QGridLayout;
    setLayout(layout);
    layout->addWidget(new QLabel("Select movie:"), 0, 0);
    layout->addWidget(m_movieList, 1, 0);

    layout->addWidget(new QLabel("Select tags"), 0, 1);
    layout->addWidget(m_tagList, 1, 1);

    QPushButton *button = new QPushButton("Save selected tags");
    layout->addWidget(button, 2, 1);
    connect(button, SIGNAL(clicked()), SLOT(save()));

    m_movieList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tagList->setSelectionMode(QAbstractItemView::MultiSelection);

    connect(m_cheggit, SIGNAL(foundMovies(const QList<QPair<QString,QUrl> >&)), SLOT(moviesFetched(const QList<QPair<QString,QUrl> >&)));
    connect(m_cheggit, SIGNAL(foundTags(const QStringList&)), SLOT(tagsFetched(const QStringList&)));
    connect(m_cheggit, SIGNAL(pleaseLogin()), SLOT(showPleaseLogin()));
    connect(m_movieList, SIGNAL(clicked(QModelIndex)), SLOT(movieSelected(QModelIndex)));

    m_cheggit->searchMovies(movieTitle);
}

void TagFetchDialog::save()
{
    foreach(QListWidgetItem *tag, m_tagList->selectedItems()) {
        Collection::addTag(m_videoName, tag->text());
    }
    close();
}

void TagFetchDialog::moviesFetched(const QList<QPair<QString, QUrl> > &movies)
{
    m_movies = movies;
    for (int i=0; i<movies.size(); i++) {
        m_movieList->addItem(movies[i].first);
    }
}

void TagFetchDialog::tagsFetched(const QStringList &tags)
{
    m_tagList->clear();
    m_tagList->addItems(tags);
    m_tagList->selectAll();
}

void TagFetchDialog::movieSelected(QModelIndex index)
{
    m_tagList->clear();
    m_cheggit->getTags(m_movies.at(index.row()).second);
}

void TagFetchDialog::showPleaseLogin()
{
    close();
    QMessageBox::warning(parentWidget(), "Unable to get usable data from Cheggit.",
                         "Please log in to Cheggit first (see the Cheggit tab).");
}
