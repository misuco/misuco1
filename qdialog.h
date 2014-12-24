#ifndef QDIALOG_H
#define QDIALOG_H

#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>

class qqdialog
{
public:
    qqdialog();
    ~qqdialog();
    int i;
private:
    QQuickView view;

};

#endif // QDIALOG_H
