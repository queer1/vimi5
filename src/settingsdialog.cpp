#include "settingsdialog.h"
#include "config.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QGridLayout>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent),
    m_collectionPath(Config::collectionPath()),
    m_collectionPathBrowse("Browse..."),
    m_maxCoverSize(QString::number(Config::maxCoverSize())),
    m_dirExplorer(Config::dirExplorer()),
    m_moviePlayer(Config::moviePlayer()),
    m_imageViewer(Config::imageViewer()),
    m_saveButton("Save"),
    m_cancelButton("Cancel")
{
    QGridLayout *formLayout = new QGridLayout;

    QFormLayout *settingsLayout = new QFormLayout;
    formLayout->addLayout(settingsLayout, 0, 0, 1, 2);

    QHBoxLayout *collectionPathLayout = new QHBoxLayout;
    collectionPathLayout->addWidget(&m_collectionPath);
    collectionPathLayout->addWidget(&m_collectionPathBrowse);
    settingsLayout->addRow("Path to collection:", collectionPathLayout);

    settingsLayout->addRow("Maximum cover size (in pixels):", &m_maxCoverSize);

    settingsLayout->addRow("File explorer (to open directories with):", &m_dirExplorer);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(&m_saveButton);
    buttonLayout->addWidget(&m_cancelButton);
    formLayout->addLayout(buttonLayout, 1, 0, 1, 1);

    setLayout(formLayout);

}
