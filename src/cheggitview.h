#ifndef CHEGGITVIEW_H
#define CHEGGITVIEW_H

#include <QWidget>
#include <QUrl>
class QWebView;
class QToolBar;
class QNetworkAccessManager;
class QUrl;
class QNetworkReply;

class CheggitView : public QWidget
{
Q_OBJECT
public:
    static CheggitView *instance() { if (!m_instance) m_instance=new CheggitView; return m_instance; }

    void searchMovies(const QString &title);
    void getTags(const QUrl &movie);


signals:
    void foundMovies(const QList<QPair<QString, QUrl> > &movies);
    void foundTags(const QStringList &tags);
    void pleaseLogin();

private slots:
    void parseSearch(QNetworkReply *reply);
    void parseMovie(QNetworkReply *reply);
    void search();

private:
    CheggitView();
    void showEvent(QShowEvent *);
    static CheggitView *m_instance;

    QToolBar *m_toolbar;
    QWebView *m_webview;
    QNetworkAccessManager *m_searchAccessManager;
    QNetworkAccessManager *m_movieAccessManager;
    bool m_searching;
};

#endif // CHEGGITVIEW_H
