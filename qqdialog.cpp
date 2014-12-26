#include <QObject>
#include "qqdialog.h"

QQDialog::QQDialog()
{
    const QString appPath = QCoreApplication::applicationDirPath();

    // This allows starting the example without previously defining QML2_IMPORT_PATH.
    QDir qmlImportDir(appPath);
#if defined (Q_OS_WIN)
    qmlImportDir.cd("..");
#endif
    qmlImportDir.cd("../../../qml");
    view=new QQuickView();
    view->engine()->addImportPath(qmlImportDir.canonicalPath());
//    QObject::connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
//    BackendHelperContext *backendContext = new BackendHelperContext(&view);
//    view.engine()->rootContext()->setContextProperty("enginioBackendContext", backendContext);
    view->setSource(QUrl("qrc:///Test.qml"));
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    view->show();
}

QQDialog::~QQDialog()
{
    view->deleteLater();
}
