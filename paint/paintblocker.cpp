/*
 
 Copyright (C) 2013 by Claudio Zopfi, Zurich, Suisse, z@x21.ch
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

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
