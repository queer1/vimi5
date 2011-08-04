#include "settingsdialog.h"
#include "config.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent),
    m_collectionPath(Config::collectionPath()),
    m_collectionPathBrowse("Browse..."),
    m_maxCoverSize(QString::number(Config::maxCoverSize())),
    m_dirExplorer(Config::dirExplorer()),
    m_moviePlayer(Config::moviePlayer()),
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
    m_maxCoverSize.setValidator(new QIntValidator);

    settingsLayout->addRow("File explorer (to open directories with):", &m_dirExplorer);
    settingsLayout->addRow("Video player:", &m_moviePlayer);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(&m_saveButton);
    buttonLayout->addWidget(&m_cancelButton);
    formLayout->addLayout(buttonLayout, 1, 0, 1, 1);

    connect(&m_collectionPathBrowse, SIGNAL(clicked()), SLOT(browseForCollection()));
    connect(&m_cancelButton, SIGNAL(clicked()), SLOT(accept()));
    connect(&m_saveButton, SIGNAL(clicked()), SLOT(save()));
    setLayout(formLayout);
}

void SettingsDialog::save()
{
    if (!m_collectionPath.text().isEmpty())
        Config::m_collectionPath = m_collectionPath.text();
    if (!m_maxCoverSize.text().isEmpty())
        Config::m_maxCoverSize = m_maxCoverSize.text().toInt();

    Config::m_dirExplorer = m_dirExplorer.text();
    Config::m_moviePlayer = m_moviePlayer.text();

    Config::save();
    accept();
}

void SettingsDialog::browseForCollection()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory Containing Collection"),
                                                    Config::collectionPath(),
                                                    QFileDialog::ShowDirsOnly);
    if (dir != "") {
        m_collectionPath.setText(dir);
    }
}
