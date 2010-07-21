#include "mainwindow.h"
#include "collectionview.h"
#include "cheggitview.h"

MainWindow::MainWindow(QWidget *parent) :
    QTabWidget(parent)
{
    setTabPosition(QTabWidget::West);
    addTab(CheggitView::instance(), QIcon(), "Cheggit");
    addTab(new CollectionView(this), QIcon(), "Collection");
}
