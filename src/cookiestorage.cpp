#include <QSettings>
#include "cookiestorage.h"

CookieStorage::CookieStorage(QObject *parent) :
    QNetworkCookieJar(parent)
{
    QSettings storage;
    setAllCookies(QNetworkCookie::parseCookies(storage.value("cookies").toByteArray()));
}

CookieStorage::~CookieStorage()
{
    QSettings storage;
    QByteArray cookies;
    foreach(QNetworkCookie cookie, allCookies()) cookies.append(cookie.toRawForm() + "\n");
    storage.setValue("cookies", cookies);
}
