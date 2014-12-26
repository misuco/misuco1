#ifndef QQDIALOG_H
#define QQDIALOG_H

#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <QCoreApplication>
#include <QDir>

#include "rc1.h"

class QQDialog
{
public:
    QQDialog();
    ~QQDialog();
    QQuickView * getView() {return view;}

private:
    QQuickView * view;
};

#endif // QQDIALOG_H
