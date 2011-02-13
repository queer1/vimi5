#include "aboutdialog.h"
#include "mainwindow.h"
#include "collectionview.h"
#include "cheggitview.h"
#include "covermaker.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("vimi alpha");

    showMaximized();
    setCentralWidget(&m_tabWidget);
    m_tabWidget.setTabPosition(QTabWidget::West);
    m_tabWidget.addTab(CheggitView::instance(), QIcon(), "Cheggit");
    CollectionView *cv = new CollectionView;
    m_tabWidget.addTab(cv, QIcon(), "Collection");
    m_tabWidget.setCurrentIndex(1);

    // Set up the File menu
    QMenu *fileMenu = new QMenu("&File", this);
    fileMenu->addAction(QIcon(), "&Rescan...", cv->collection(), SLOT(rescan()), QKeySequence::Refresh);
    fileMenu->addAction(QIcon(), "&Set Path to Collection...", this, SLOT(getCollectionPath()));
    fileMenu->addAction(QIcon(), "&Quit", this, SLOT(close()), QKeySequence::Quit);
    menuBar()->addMenu(fileMenu);

    // Set up the Help menu
    QMenu *helpMenu = new QMenu("&Help", this);
    helpMenu->addAction(QIcon(), "&About Vimi...", this, SLOT(showAboutDialog()));
    menuBar()->addMenu(helpMenu);

    if (Config::collectionPath().isEmpty()) {
        getCollectionPath();
    }
    statusBar()->showMessage(tr("Ready."));
}

void MainWindow::showAboutDialog()
{
    AboutDialog *dialog = new AboutDialog(this);
    dialog->show();
}


void MainWindow::getCollectionPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory Containing Collection"),
                                                    Config::collectionPath(),
                                                    QFileDialog::ShowDirsOnly);
    if (dir != "") {
        Config::setCollectionPath(dir);
    }
}
