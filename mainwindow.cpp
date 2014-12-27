#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    //QWidget * top = new QWidget();
    //QGridLayout * g = new QGridLayout();
    //top->setLayout(g);

    w = new RC1(this);
    setCentralWidget(w);
    w->show();

    //DialogNet * dn=new DialogNet();

    //g->setHorizontalSpacing(0);
    //g->setVerticalSpacing(0);
    //g->addWidget(w,0,0);
    //g->addWidget(dn,1,0);
    //g->setRowMinimumHeight(0,400);

    //w->connectApp(&a);
    //top->connectApp(&a);
    //w->show();
    //top->show();
}

void MainWindow::connectApp(QApplication *a)
{
    w->connectApp(a);
}
