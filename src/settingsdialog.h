#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);

signals:

private slots:
    void browseForCollection();
    void save();

private:
    QLineEdit m_collectionPath;
    QPushButton m_collectionPathBrowse;

    //QListWidget m_movieSuffixList;

    QLineEdit m_maxCoverSize;
    QLineEdit m_dirExplorer;
    QLineEdit m_moviePlayer;

    QPushButton m_saveButton;
    QPushButton m_cancelButton;
};

#endif // SETTINGSDIALOG_H
