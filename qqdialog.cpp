#include <QObject>
#include <QQuickItem>
#include "qqdialog.h"

QQDialog::QQDialog()
{
    view=new QQuickView();
    view->setSource(QUrl("qrc:///NetDialog.qml"));
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    view->show();
}

QQDialog::~QQDialog()
{
    view->deleteLater();
}

void QQDialog::setContent(QString adr, int port)
{
    QQuickItem * i=view->rootObject();
    QObject * p = i->findChild<QObject*>("adr");
    if(p) {
        p->setProperty("text",adr);
        qDebug() << "set property text";
    }
    p = i->findChild<QObject*>("port");
    if(p) {
        p->setProperty("text",port);
        qDebug() << "set property port";
    }
}
