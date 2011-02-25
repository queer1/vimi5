#include "covermaker.h"
#include <QDebug>
#include <QPushButton>
#include "collection.h"
#include <QVBoxLayout>
#include <QImageWriter>

CoverMaker::CoverMaker(QString videoName, QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Create covers for " + videoName);
    setLayout(new QVBoxLayout(this));

    m_path = Collection::getPath(videoName);
    if (Collection::getFiles(videoName).size() > 1) {
        m_comboBox = new QComboBox(this);
        m_comboBox->addItems(Collection::getFiles(videoName));
        layout()->addWidget(m_comboBox);
        connect(m_comboBox, SIGNAL(activated(QString)), this, SLOT(selectVideo(QString)));
    }

    m_videoWidget = new VideoWidget(this, m_path + "/" +  Collection::getFiles(videoName).first());
    //m_videoWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_slider = new QSlider(this);
    m_slider->setMaximum(m_videoWidget->length());
    m_slider->setTracking(true);
    m_slider->setOrientation(Qt::Horizontal);
    m_slider->setValue(m_slider->maximum() / 2);

    QPushButton *saveButton = new QPushButton("Save");


    layout()->addWidget(saveButton);
    layout()->addWidget(m_slider);
    layout()->addWidget(m_videoWidget);

    connect(m_slider, SIGNAL(valueChanged(int)), m_videoWidget, SLOT(seek(int)));
    connect(saveButton, SIGNAL(clicked()), SLOT(saveFrame()));
}

void CoverMaker::selectVideo(QString file) {
    delete m_videoWidget;
    m_videoWidget = new VideoWidget(this, m_path + "/" + file);
    layout()->addWidget(m_videoWidget);
    m_slider->setMaximum(m_videoWidget->length());
    m_slider->setValue(0);
    connect(m_slider, SIGNAL(valueChanged(int)), m_videoWidget, SLOT(seek(int)));
}

void CoverMaker::saveFrame()
{
    QImage frame = m_videoWidget->getFrame();
    QDir dir(m_path);
    QString filename = "cover";
    if (dir.exists(filename + ".jpg")) {
        QString newName;
        int i = 0;
        do {
            newName = filename + QString::number(i);
            i++;
        } while (dir.exists(newName + ".jpg"));
        filename = newName;
    }
    QImageWriter writer (m_path + "/" + filename + ".jpg");
    writer.write(frame);
}
