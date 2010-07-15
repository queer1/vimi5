#ifndef CHEGGITVIEW_H
#define CHEGGITVIEW_H

#include <QNetworkReply>
#include <QWebView>
#include <QWidget>
#include <QToolBar>

class CheggitView : public QWidget
{
Q_OBJECT
public:
    explicit CheggitView(QWidget *parent = 0);

    void searchMovies(const QString &title);
    void getTags(const QUrl &movie);

signals:
    void foundMovies(const QList<QPair<QString, QUrl> > movies);
    void foundTags(const QStringList tags);

private slots:
    void parseSearch(QNetworkReply *reply);
    void parseMovie(QNetworkReply *reply);
    void search();

private:
    QToolBar *m_toolbar;
    QWebView *m_webview;
    QNetworkAccessManager *m_searchAccessManager;
    QNetworkAccessManager *m_movieAccessManager;
    bool m_searching;
};

#endif // CHEGGITVIEW_H
