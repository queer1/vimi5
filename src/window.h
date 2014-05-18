#ifndef WINDOW_H
#define WINDOW_H

#include <QQuickView>

class Window : public QQuickView
{
    Q_OBJECT

signals:

public slots:
    void toggleVisible() { setVisible(!isVisible()); }

};

#endif // WINDOW_H
