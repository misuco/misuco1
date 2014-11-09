#include "paintblocker.h"

PaintBlocker::PaintBlocker()
{
}

void PaintBlocker::paint(RC1 *view, QPainter *pnt)
{
    
    pnt->drawImage(0,0,*(view->getBgImage()),0,0,view->getLayout()->getWidth(),view->getLayout()->getHeight());
    closeWidth=view->height()/5-10;
    closeXpos=view->width()-closeWidth-5;
    
    pnt->setBrush(Qt::NoBrush);
    pnt->setPen(QPen(Qt::lightGray,4));
    pnt->drawEllipse(closeXpos,5,closeWidth,closeWidth);
    //pnt->setPen(Qt::black);
    pnt->setFont(QFont(view->getLayout()->getFont(),view->getLayout()->getFontsize()));

    QString text;
    text.sprintf("Ad: click to open OR wait %d seconds OR press  → ",view->getBlockerTimeLeft());
    pnt->drawText(5,5,view->getLayout()->getWidth(),view->getLayout()->getHeight(),Qt::AlignLeft,text);

    pnt->drawText(closeXpos,5,closeWidth,closeWidth,Qt::AlignCenter,"skip");
    // pnt->drawText(5,5,view->getLayout()->getWidth(),closeWidth,Qt::AlignLeft,"advertisement");
}
