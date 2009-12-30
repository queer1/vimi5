#include "aboutdialog.h"

#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    setLayout(new QVBoxLayout(this));

    m_header = new QLabel("<h2>Vimi</h2>tag based video manager", this);
    m_text = new QLabel("Copyright 2009 cwk", this);
    m_header->setAlignment(Qt::AlignCenter);
    m_text->setAlignment(Qt::AlignCenter);

    m_button = new QPushButton("Close");

    layout()->addWidget(m_header);
    layout()->addWidget(m_text);
    layout()->addWidget(m_button);

    setWindowTitle("About...");

    connect(m_button, SIGNAL(clicked()), this, SLOT(accept()));
}
