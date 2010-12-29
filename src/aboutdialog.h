#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);

private:
    QLabel *m_text;
    QLabel *m_header;
    QLabel *m_logo;
    QPushButton *m_button;
};

#endif // ABOUTDIALOG_H
