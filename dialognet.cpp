#include "dialognet.h"

DialogNet::DialogNet(QWidget *parent) :
    QWidget(parent)
{
    layout = new QGridLayout();
    //lineEdit = new QLineEdit();
    lineEdit = new QTextEdit();
    button = new QPushButton();
    layout->addWidget(lineEdit,0,1);
    layout->addWidget(button,0,2);
    setLayout(layout);

}
