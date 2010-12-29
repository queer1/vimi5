#ifndef COOKIESTORAGE_H
#define COOKIESTORAGE_H

#include <QNetworkCookieJar>

class CookieStorage : public QNetworkCookieJar
{
Q_OBJECT
public:
    explicit CookieStorage(QObject *parent = 0);
    ~CookieStorage();
};

#endif // COOKIESTORAGE_H
