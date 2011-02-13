#ifndef TAGDIALOG_H
#define TAGDIALOG_H

#include "collection.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

class TagDialog : public QDialog
{
Q_OBJECT
public:
    explicit TagDialog(const QString &videoName, QWidget *parent = 0);

private slots:
    void addTag();
    void removeTag();

private:
    void updateModel();


    QComboBox *m_tagEdit;
    QListWidget *m_tagView;
    QPushButton *m_addButton;
    QPushButton *m_removeButton;
    QPushButton *m_closeButton;
    QLabel *m_title;
    QString m_videoName;
    Collection *m_collection;
};

#endif // TAGDIALOG_H
