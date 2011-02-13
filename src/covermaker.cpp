#include "covermaker.h"
#include <QDebug>
#include <QPushButton>
#include "collection.h"
#include <QVBoxLayout>
#include <QImageWriter>

CoverMaker::CoverMaker(QString videoName) : QDialog()
{
    m_path = Collection::getPath(videoName);
    m_comboBox = new QComboBox(this);
    m_comboBox->addItems(Collection::getFiles(videoName));

    m_videoWidget = new VideoWidget(this, m_path + "/" +  Collection::getFiles(videoName).first());
    m_videoWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_videoWidget->show();
    m_slider = new QSlider(this);
    m_slider->setMaximum(m_videoWidget->length());
    m_slider->setTracking(true);
    m_slider->setOrientation(Qt::Horizontal);

    QPushButton *saveButton = new QPushButton("Save");

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(m_comboBox);
    layout()->addWidget(saveButton);
    layout()->addWidget(m_videoWidget);
    layout()->addWidget(m_slider);

    connect(m_slider, SIGNAL(valueChanged(int)), m_videoWidget, SLOT(seek(int)));
    connect(m_comboBox, SIGNAL(activated(QString)), this, SLOT(selectVideo(QString)));
    connect(saveButton, SIGNAL(clicked()), SLOT(saveFrame()));
}

void CoverMaker::selectVideo(QString file) {
    delete m_videoWidget;
    m_videoWidget = new VideoWidget(this, m_path + "/" + file);
    layout()->addWidget(m_videoWidget);
    layout()->removeWidget(m_slider); // uuglyyy
    layout()->addWidget(m_slider);
    m_slider->setMaximum(m_videoWidget->length());
    m_slider->setValue(0);
    connect(m_slider, SIGNAL(valueChanged(int)), m_videoWidget, SLOT(seek(int)));
}

void CoverMaker::saveFrame()
{
    QImage frame = m_videoWidget->getFrame();
    QDir dir(m_path);
    QString filename = "cover";
    if (dir.exists(filename)) {
        QString newName;
        int i = 0;
        do {
            newName = filename + i;
            i++;
        } while (dir.exists(newName));
        filename = newName;
    }
    QImageWriter writer (m_path + "/" + filename + ".jpg");
    writer.write(frame);
}
