#include "aboutdialog.h"
#include "mainwindow.h"
#include "collectionview.h"
#include "cheggitview.h"
#include "covermaker.h"
#include "settingsdialog.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QApplication>
#include <QThreadPool>
#include <QWaitCondition>
#include <QSettings>

#include <QDebug>

bool MainWindow::running;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    running = true;

    //hide();
    setWindowTitle("vimi alpha");
    setWindowIcon(QIcon(":/images/icon.png"));

    //showMaximized();
    setCentralWidget(&m_tabWidget);
    m_tabWidget.setTabPosition(QTabWidget::West);
    m_tabWidget.addTab(CheggitView::instance(), QIcon(), "Cheggit");
    CollectionView *cv = new CollectionView;
    m_tabWidget.addTab(cv, QIcon(), "Collection");
    m_tabWidget.setCurrentIndex(1);

    // Set up the File menu
    QMenu *fileMenu = new QMenu("&File", this);
    fileMenu->addAction(QIcon(), "&Rescan...", cv->collection(), SLOT(rescan()), QKeySequence::Refresh);
    fileMenu->addAction(QIcon(), "&Settings...", this, SLOT(showSettings()), QKeySequence::Preferences);
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

    //statusBar()->showMessage(tr("Ready."));
    connect(cv, SIGNAL(statusUpdated(QString)), statusBar(), SLOT(showMessage(QString)));

    QMetaObject::invokeMethod(this, "showMaximized");
    Collection::launched = true;
    Collection::launchWaiter.wakeAll();

    QSettings settings("Cheggit", "Vimi");
    restoreGeometry(settings.value("geometry").toByteArray());
}

MainWindow::~MainWindow()
{
    running = false;
    QApplication::quit();
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

void MainWindow::showSettings()
{
    SettingsDialog *dialog = new SettingsDialog(this);
    dialog->show();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("Cheggit", "Vimi");
    settings.setValue("geometry", saveGeometry());
    QWidget::closeEvent(event);
}
