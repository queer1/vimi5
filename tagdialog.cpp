#include "tagdialog.h"

#include "collection.h"

#include <QCompleter>
#include <QDebug>
#include <QLineEdit>
#include <QSpacerItem>
#include <QVBoxLayout>

TagDialog::TagDialog(const QString &videoName, Collection *collection, QWidget *parent) :
    QDialog(parent), m_collection(collection)
{
    m_videoName = videoName;

    setWindowTitle("Editing tags for video '" + videoName + "'.");

    m_title = new QLabel("Tags for '" + videoName + "'.");
    m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_tagEdit = new QComboBox(this);
    m_tagEdit->setEditable(true);
    m_tagEdit->setCompleter(new QCompleter(Collection::getTags(), this));
    m_tagEdit->completer()->setCaseSensitivity(Qt::CaseInsensitive);

    m_tagView = new QListWidget(this);
    m_tagView->setSelectionMode(QAbstractItemView::SingleSelection);
    updateModel();

    m_addButton = new QPushButton("Add", this);
    m_removeButton = new QPushButton("Remove", this);
    m_closeButton = new QPushButton("Close", this);

    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);
    layout->addWidget(m_title, 0, 0, 1, 2);

    layout->addWidget(m_tagEdit, 1, 0);
    layout->addWidget(m_tagView, 2, 0, 3, 1);

    layout->addItem(new QSpacerItem(QSizePolicy::Minimum, QSizePolicy::Expanding), 3, 1);

    layout->addWidget(m_addButton, 1, 1);
    layout->addWidget(m_removeButton, 2, 1);
    layout->addWidget(m_closeButton, 4, 1);

    connect(m_removeButton, SIGNAL(clicked()), this, SLOT(removeTag()));
    connect(m_addButton, SIGNAL(clicked()), this, SLOT(addTag()));
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(accept()));
        qDebug() << m_videoName;
}

void TagDialog::updateModel()
{
    m_tagView->clear();
    m_tagView->insertItems(0, Collection::getTags(m_videoName));
}

void TagDialog::addTag()
{
    m_collection->addTag(m_videoName, m_tagEdit->lineEdit()->text());
    m_tagEdit->lineEdit()->clear();

    updateModel();
}

void TagDialog::removeTag()
{
    qDebug() << m_videoName;
    QString tag = m_tagView->currentItem()->text(); //m_tagModel->data(m_tagView->currentIndex()).toString();
    m_collection->removeTag(m_videoName, tag);

    updateModel();
}
