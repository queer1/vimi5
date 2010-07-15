#include "cheggitview.h"
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDomDocument>
#include <QPair>



CheggitView::CheggitView(QWidget *parent) :
    QWidget(parent), m_searching(false)
{
    m_webview = new QWebView(this);

    m_toolbar = new QToolBar("Actions", this);
    m_toolbar->addAction(m_webview->pageAction(QWebPage::Back));
    m_toolbar->addAction(m_webview->pageAction(QWebPage::Forward));
    m_toolbar->addAction(m_webview->pageAction(QWebPage::Reload));
    m_toolbar->addAction(m_webview->pageAction(QWebPage::Stop));

    setLayout(new QVBoxLayout);
    layout()->addWidget(m_toolbar);
    layout()->addWidget(m_webview);

    m_webview->load(QUrl("http://cheggit.net"));

    QPushButton *button = new QPushButton("test!", this);
    connect(button, SIGNAL(clicked()), SLOT(search()));
    layout()->addWidget(button);

    m_movieAccessManager = new QNetworkAccessManager(this);
    m_movieAccessManager->setCookieJar(m_webview->page()->networkAccessManager()->cookieJar());
    m_searchAccessManager = new QNetworkAccessManager(this);
    m_searchAccessManager->setCookieJar(m_webview->page()->networkAccessManager()->cookieJar());
}

void CheggitView::search()
{
    searchMovies("cumshots");
}

void CheggitView::searchMovies(const QString &title)
{
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
        QList<QPair<QString, QUrl> > urls;
        QByteArray request = reply->readAll();
        if (request.isEmpty()) {
            qWarning() << "Read no data from cheggit!";
            return;
        }
        qDebug() << request;
        QString error;
        QDomDocument doc("search");
        if (!doc.setContent(request, false, &error)) {
            qWarning() << error;
        }

        QString urlBeginning("http://cheggit.net/");
        QString url;
        QString title;

        QDomNode node = doc.documentElement().firstChild();
        while (!node.isNull()) {
            QDomElement element = node.toElement();
            qDebug() << element.tagName() << element.attribute("href");
            if (!element.isNull() && element.tagName() == "a" && element.attribute("href").startsWith("torrents.php?torrentid=")) {
                url = element.attribute("href");
                title = element.text();
                qDebug() << title << url;
                urls.append(QPair<QString, QUrl>(title, QUrl(urlBeginning + url)));
            }
            node = node.nextSibling();
        }
        if (!urls.isEmpty())
            emit foundMovies(urls);
    }
}

void CheggitView::getTags(const QUrl &movie)
{
}

void CheggitView::parseMovie(QNetworkReply *reply)
{

}
