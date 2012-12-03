/*
 * Application main window class
 * Copyright (C) 2010-2012 cwk <coolwk@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QModelIndex;
class QListView;
class QTreeView;
class QStandardItemModel;
class QSortFilterProxyModel;
class QLineEdit;
class VideoFilterProxyModel;
class InfoPanel;
class QStandardItem;

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static bool running;

public slots:
    void getCollectionPath();
    void showSettings();
    void showTagReplacementDialog();
    void showFavouriteTagDialog();

signals:
    void updatedTags();

private slots:
    void showAboutDialog();
    void updateTagModel();
    void updateVideoFilter(QStandardItem *tag);
    void updateInfoPanel(const QModelIndex&);
    void editTags();
    void createCovers();
    void selectTag(const QString &tag);

private:
    void closeEvent(QCloseEvent *);

    QListView *m_tagView;
    QTreeView *m_videoView;
    QStandardItemModel *m_tagModel;
    QStandardItemModel *m_favouriteTagModel;
    QSortFilterProxyModel *m_tagFilterModel;
    QLineEdit *m_tagFilterEdit;
    QLineEdit *m_videoFilterEdit;
    VideoFilterProxyModel *m_videoModel;
    InfoPanel *m_infoPanel;
    QListView *m_favouriteTags;
};

#endif // MAINWINDOW_H
