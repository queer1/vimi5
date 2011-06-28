#include "cheggitview.h"
#include "cookiestorage.h"
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QRegExp>
#include <QPair>

CheggitView *CheggitView::m_instance=0;

CheggitView::CheggitView() : QWidget(0),
    m_webview(new QWebView(this)),
    m_searching(false)
{
    CookieStorage *cookieJar = new CookieStorage(this);
    m_webview->page()->networkAccessManager()->setCookieJar(cookieJar);

    m_toolbar = new QToolBar("Actions", this);
    m_toolbar->addAction(m_webview->pageAction(QWebPage::Back));
    m_toolbar->addAction(m_webview->pageAction(QWebPage::Forward));
    m_toolbar->addAction(m_webview->pageAction(QWebPage::Reload));
    m_toolbar->addAction(m_webview->pageAction(QWebPage::Stop));

    setLayout(new QVBoxLayout);
    layout()->addWidget(m_toolbar);
    layout()->addWidget(m_webview);

    //m_webview->load(QUrl("http://cheggit.net"));


    m_movieAccessManager = new QNetworkAccessManager(this);
    m_movieAccessManager->setCookieJar(cookieJar);
    m_searchAccessManager = new QNetworkAccessManager(this);
    m_searchAccessManager->setCookieJar(cookieJar);
}

void CheggitView::search()
{
    //searchMovies("cumshots");
    getTags(QUrl("http://cheggit.net/torrents.php?torrentid=112758"));
}

void CheggitView::searchMovies(const QString &t)
{
    QString title(t);
    title.replace('.', ' ');
    QUrl url("http://cheggit.net/browsetorrents.php");
    QString filter("title:[");
    filter += title + "]";
    url.addQueryItem("filter", filter);

    qDebug() << "Searching for movies at " << url;

    connect(m_searchAccessManager, SIGNAL(finished(QNetworkReply*)), SLOT(parseSearch(QNetworkReply*)));
    m_searchAccessManager->get(QNetworkRequest(url));
}

void CheggitView::parseSearch(QNetworkReply *reply)
{
    qDebug() << "Got search reply";

    disconnect(this, SLOT(parseSearch(QNetworkReply*)));
    if (!reply->error()) {
        QString request = QString(reply->readAll());
        if (request.isEmpty()) {
            qWarning() << "Read no data from cheggit!";
            return;
        }

        QRegExp regexp("(torrents.php\\?torrentid=[0-9]+)\">([^<]+)");
        QList<QPair<QString, QUrl> > urls;
        QString urlBeginning("http://cheggit.net/");
        QString url;
        QString title;

        int pos = 0;

        while ((pos = regexp.indexIn(request, pos)) != -1) {
            url = regexp.cap(1);
            title = regexp.cap(2);
            pos += regexp.matchedLength();
            if (url.isEmpty() || title.isEmpty()) continue;
            //qDebug() << title << url;
            urls.append(QPair<QString, QUrl>(title, QUrl(urlBeginning + url)));
        }
        if (!urls.isEmpty())
            emit foundMovies(urls);
    }
}

void CheggitView::getTags(const QUrl &movie)
{
    connect(m_movieAccessManager, SIGNAL(finished(QNetworkReply*)), SLOT(parseMovie(QNetworkReply*)));
    m_movieAccessManager->get(QNetworkRequest(movie));
}

void CheggitView::parseMovie(QNetworkReply *reply)
{
    qDebug() << "Got movie reply";

    disconnect(this, SLOT(parseSearch(QNetworkReply*)));
    if (!reply->error()) {
        QString request = QString(reply->readAll());
        if (request.isEmpty()) {
            qWarning() << "Read no data from cheggit!";
            return;
        }

        QRegExp regexp("tags\\.php\\?filter=([^\"]+)");
        QStringList tags;
        QString tag;

        int pos = 0;

        while ((pos = regexp.indexIn(request, pos)) != -1) {
            tag = regexp.cap(1);
            pos += regexp.matchedLength();
            if (tag.isEmpty()) continue;
            //qDebug() << tag;
            tag.replace('.', ' ');
            tags << tag;
        }

        if (!tags.isEmpty())
            emit foundTags(tags);
    }
}
