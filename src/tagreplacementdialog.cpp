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
