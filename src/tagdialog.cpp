#include "tagdialog.h"

#include "collection.h"

#include <QCompleter>
#include <QDebug>
#include <QLineEdit>
#include <QSpacerItem>
#include <QVBoxLayout>

TagDialog::TagDialog(const QString &videoName, QWidget *parent) :
    QDialog(parent)
{
    m_videoName = videoName;

    setWindowTitle("Editing tags for video '" + videoName + "'.");

    m_title = new QLabel("Tags for '" + videoName + "'.");
    m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_tagEdit = new QComboBox(this);
    m_tagEdit->setEditable(true);
    m_tagEdit->setCompleter(new QCompleter(Collection::getTags().toList(), this));
    m_tagEdit->completer()->setCaseSensitivity(Qt::CaseInsensitive);
    m_tagEdit->completer()->setCompletionMode(QCompleter::InlineCompletion);

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
    connect(m_tagEdit, SIGNAL(activated(QString)), this, SLOT(addTag(QString)));
}

void TagDialog::updateModel()
{
    QStringList tags = Collection::getTags(m_videoName).toList();
    qSort(tags);
    m_tagView->clear();
    m_tagView->insertItems(0, tags);
}

void TagDialog::addTag(QString tag)
{
    Collection::addTag(m_videoName, tag);
    m_tagEdit->clearEditText();
    updateModel();
}

void TagDialog::removeTag()
{
    QString tag = m_tagView->currentItem()->text();
    Collection::removeTag(m_videoName, tag);

    updateModel();
}
