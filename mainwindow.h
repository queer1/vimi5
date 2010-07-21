#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTabWidget>
#include <QMainWindow>

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void showAboutDialog();
    void getCollectionPath();

private:
    QTabWidget m_tabWidget;
};

#endif // MAINWINDOW_H
