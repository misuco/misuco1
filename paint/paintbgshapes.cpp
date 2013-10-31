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
#include "paintbgshapes.h"

PaintBgShapes::PaintBgShapes()
{
    sPenAct=160;
    lPenAct=0;
    sBrushAct=160;
    lBrushAct=200;

    sPenPsv=160;
    lPenPsv=0;
    sBrushPsv=160;
    lBrushPsv=80;
}

void PaintBgShapes::paint(RC1 *view, QPainter * pnt) {

    LayoutModel * lay=view->getLayout();

    int iseg=0;
    int xpaint=0;
    int ypaint=0;
    int xpaint1=0;
    int ypaint1=0;

    pnt->setPen(Qt::NoPen);
    for(int y = 0; y < lay->getNrows(); y++) {
        ypaint1=lay->getRowheightpx(y);
        xpaint=0;

        for (int x = 0; x < lay->getNseg(y); x ++) {
            xpaint1=lay->getSegwidthpx(iseg);
//            int col=21*(lay->getNote(iseg)%12);
            int col=lay->getSegH(iseg);
            int lightP=lPenPsv;
            int lightB=lBrushPsv;
            int satP=sPenPsv;
            int satB=sBrushPsv;

            if(lay->getPressed(iseg) > 0) {
                lightP=lPenAct;
                lightB=lBrushAct;
                satP=sPenAct;
                satB=sBrushAct;
            }

            if(lay->getSegtype(iseg)==1) {
                QLinearGradient linearGrad(QPointF(xpaint, ypaint), QPointF(xpaint+xpaint1, ypaint));
                linearGrad.setColorAt(0, Qt::black);
                linearGrad.setColorAt(1, Qt::white);
                pnt->setBrush(linearGrad);
            } else {
                if(lightB>0) {
                    pnt->setBrush(QColor::fromHsl(col,satB,lightB));
                } else {
                    pnt->setBrush(Qt::NoBrush);
                }
            }
            if(lightP>0) {
                pnt->setPen(QColor::fromHsl(col,satP,lightP));
            } else {
                pnt->setPen(Qt::NoPen);
            }

            pnt->drawRect(xpaint,ypaint,xpaint1,ypaint1);

            pnt->setPen(QColor::fromHsl((col+127)%255,100,100));
            pnt->setFont(QFont("Ubuntu",20));
            pnt->drawText(xpaint,ypaint,xpaint1,ypaint1,Qt::AlignCenter,*lay->getSegText(iseg));

            xpaint+=xpaint1;
            iseg++;
        }
        ypaint+=ypaint1;
    }
}

int PaintBgShapes::getParamCount() {
    return 8;
}

float PaintBgShapes::getParam(quint16 n) {
    return 0;
}

float PaintBgShapes::getParamMin(quint16 n) {
    return 0;
}

float PaintBgShapes::getParamMax(quint16 n) {
    return 0;
}

void PaintBgShapes::setParam(quint16 n, float v) {
    switch(n) {
    case 0:
        sPenAct=v;
        break;
    case 1:
        lPenAct=v;
        break;
    case 2:
        sPenPsv=v;
        break;
    case 3:
        lPenPsv=v;
        break;
    case 4:
        sBrushAct=v;
        break;
    case 5:
        lBrushAct=v;
        break;
    case 6:
        sBrushPsv=v;
        break;
    case 7:
        lBrushPsv=v;
    }
}
