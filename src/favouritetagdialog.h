#ifndef FAVOURITETAGDIALOG_H
#define FAVOURITETAGDIALOG_H

#include <QDialog>
class QListWidget;
class QComboBox;
class FavouriteTagDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FavouriteTagDialog(QWidget *parent = 0);
    
signals:
    
private slots:
    void updateModel();
    void addTag(QString tag);
    void removeTag();

private:
    QListWidget *m_tagView;
    QComboBox *m_tagEdit;
};

#endif // FAVOURITETAGDIALOG_H
