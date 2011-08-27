#ifndef TAGREPLACEMENTDIALOG_H
#define TAGREPLACEMENTDIALOG_H

#include <QDialog>

class QComboBox;

class TagReplacementDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TagReplacementDialog(QWidget *parent = 0);

private slots:
    void doChange();

private:
    QComboBox *m_existingTag;
    QComboBox *m_newTag;
};

#endif // TAGREPLACEMENTDIALOG_H
