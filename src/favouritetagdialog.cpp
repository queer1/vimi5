#include "favouritetagdialog.h"
#include <QLabel>
#include <QComboBox>
#include <QListWidget>
#include <QCompleter>
#include <QPushButton>
#include <QGridLayout>
#include "collection.h"
#include "config.h"

FavouriteTagDialog::FavouriteTagDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Editing favourite tags");

    QLabel *title = new QLabel("Favourite tags:");
    title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_tagEdit = new QComboBox(this);
    m_tagEdit->setEditable(true);
    m_tagEdit->setCompleter(new QCompleter(Collection::getTags(), this));
    m_tagEdit->completer()->setCaseSensitivity(Qt::CaseInsensitive);
    m_tagEdit->completer()->setCompletionMode(QCompleter::InlineCompletion);

    m_tagView = new QListWidget(this);
    m_tagView->setSelectionMode(QAbstractItemView::SingleSelection);
    updateModel();

    QPushButton *addButton = new QPushButton("Add", this);
    QPushButton *removeButton = new QPushButton("Remove", this);
    QPushButton *closeButton = new QPushButton("Close", this);

    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);
    layout->addWidget(title, 0, 0, 1, 2);

    layout->addWidget(m_tagEdit, 1, 0);
    layout->addWidget(m_tagView, 2, 0, 3, 1);

    layout->addItem(new QSpacerItem(QSizePolicy::Minimum, QSizePolicy::Expanding), 3, 1);

    layout->addWidget(addButton, 1, 1);
    layout->addWidget(removeButton, 2, 1);
    layout->addWidget(closeButton, 4, 1);

    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeTag()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(addTag(QString)));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_tagEdit, SIGNAL(activated(QString)), this, SLOT(addTag(QString)));
}

void FavouriteTagDialog::updateModel()
{
    QStringList tags = Config::favouriteTags();
    m_tagView->clear();
    m_tagView->insertItems(0, tags);

}


void FavouriteTagDialog::addTag(QString tag)
{
    Config::addFavouriteTag(tag);
    m_tagEdit->clearEditText();
    updateModel();
}

void FavouriteTagDialog::removeTag()
{
    QString tag = m_tagView->currentItem()->text();
    Config::removeFavouriteTag(tag);

    updateModel();
}
