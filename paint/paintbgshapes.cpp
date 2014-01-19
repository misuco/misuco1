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
#include <QColor>

PaintBgShapes::PaintBgShapes()
{
    sPenAct=160;
    lPenAct=200;
    sBrushAct=160;
    lBrushAct=200;
    
    sPenPsv=160;
    lPenPsv=50;
    sBrushPsv=160;
    lBrushPsv=80;
    
    colorMode=1;
    chue=0;
    
    cradx=10;
    crady=20;
    
    gradients=1;
    painttext=1;
}

void PaintBgShapes::paint(RC1 *view, QPainter * pnt) {
    
    LayoutModel * lay=view->getLayout();
    
    int iseg=0;
    int col=chue;
    int xpaint=0;
    int ypaint=0;
    int xpaint1=0;
    int ypaint1=0;
    /*
    int avg_cnt=0;
    int sum_r=0;
    int sum_g=0;
    int sum_b=0;
    */
    pnt->setPen(Qt::NoPen);
    pnt->setBrush(Qt::black);
    pnt->drawRect(0,0,view->width(),view->height());
    
    /*
     for(int y = 0; y < lay->getNrows(); y++) {
     for (int x = 0; x < lay->getNseg(y); x ++) {
     if(lay->getPressed(iseg)) {
     int col;
     if(lay->getSegtype(iseg)==1 && iseg>0 && iseg+1<lay->getNseg(y)) {
     int col1;
     int col2;
     if(colorMode==1 ) {
     col1=(lay->getMidiNote(iseg-1)%12)*32;
     col2=(lay->getMidiNote(iseg+1)%12)*32;
     } else {
     col1=lay->getSegH(iseg-1);
     col2=lay->getSegH(iseg+1);
     }
     if(col2>col1) {
     col=col1+((col2-col1)*lay->getSegH(iseg))/256;
     } else {
     col=col2+((col1-col2)*lay->getSegH(iseg))/256;
     }
     //                    qDebug() << col << lay->getSegH(iseg);
     } else {
     if(colorMode==1 ) {
     col=32*(lay->getMidiNote(iseg)%12);
     } else {
     col=lay->getSegH(iseg);
     }
     }
     int lightB=lBrushPsv;
     int satB=sBrushPsv;
     QColor c1=QColor::fromHsl(col,satB,lightB);
     sum_r+=c1.red();
     sum_g+=c1.green();
     sum_b+=c1.blue();
     avg_cnt++;
     }
     iseg++;
     }
     }
     
     if(avg_cnt>0) {
     sum_r/=avg_cnt;
     sum_g/=avg_cnt;
     sum_b/=avg_cnt;
     pnt->setBrush(QColor::fromRgb(sum_r,sum_g,sum_b));
     pnt->drawRect(0,0,view->width(),view->height());
     }
     */
    
    iseg=0;
    pnt->setBrush(Qt::NoBrush);
    
    for(int y = 0; y < lay->getNrows(); y++) {
        ypaint1=lay->getRowheightpx(y);
        xpaint=1;
        
        for (int x = 0; x < lay->getNseg(y); x ++) {
            xpaint1=lay->getSegwidthpx(iseg);
            
            int xpaint1_1=xpaint1-1;
            int ypaint1_1=ypaint1-1;
            int ypaint1_2=ypaint1-2*crady;
            if(colorMode==1 ) {
                col=32*(lay->getMidiNote(iseg)%12);
            } else if(colorMode==2 ){
                col=lay->getSegH(iseg);
            }
            
            //            int col=21*(lay->getNote(iseg)%12);
            //            int col=lay->getSegH(iseg);
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
            
            //            int col1=lay->getSegH(iseg-1);
            //            int col2=lay->getSegH(iseg+1);
            int col1=chue;
            int col2=chue;
            if(lay->getSegtype(iseg)==1) {
                if(colorMode==1 ) {
                    col1=(lay->getMidiNote(iseg-1)%12)*32;
                    col2=(lay->getMidiNote(iseg+1)%12)*32;
                } else if(colorMode==2 ) {
                    col1=lay->getSegH(iseg-1);
                    col2=lay->getSegH(iseg+1);
                }
                
                if(gradients>0) {
                    QLinearGradient linearGrad(QPointF(xpaint, ypaint), QPointF(xpaint+xpaint1, ypaint));
                    if(iseg>0) {
                        linearGrad.setColorAt(0, QColor::fromHsl(col1,satB,lightB));
                        linearGrad.setColorAt(1, QColor::fromHsl(col2,satB,lightB));
                    } else {
                        linearGrad.setColorAt(0, Qt::black);
                        linearGrad.setColorAt(1, Qt::white);
                        
                    }
                    pnt->setBrush(linearGrad);
                }
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
            /*
             if(lay->getPressed(iseg)) {
             QColor c1=QColor::fromHsl(col,satB,lightB);
             //                qDebug() << col << " " << satB << " " << lightB;
             sum_r+=c1.red();
             sum_g+=c1.green();
             sum_b+=c1.blue();
             avg_cnt++;
             //                qDebug() << sum_r << " " << sum_g << " " << sum_b;
             }
             */
            
            if(lay->getSegtype(iseg)==1) {
                if(gradients) {
                    pnt->drawRect(xpaint,ypaint+crady,xpaint1_1,ypaint1_2);
                } else {
                    if(lightB>0) {
                        pnt->setBrush(QColor::fromHsl(col1,satB,lightB));
                    } else {
                        pnt->setBrush(Qt::NoBrush);
                    }
                    
                    QPoint points1[4] = {
                        QPoint(xpaint, ypaint+crady),
                        QPoint(xpaint+xpaint1-2, ypaint+crady),
                        QPoint(xpaint, ypaint+ypaint1_2),
                        QPoint(xpaint, ypaint+crady)
                    };
                    
                    pnt->setPen(QColor::fromHsl(col1,satP,lightP));
                    pnt->drawPolygon(points1,4);
                    
                    QPoint points2[4] = {
                        QPoint(xpaint+xpaint1-1, ypaint+crady),
                        QPoint(xpaint+xpaint1-1, ypaint+ypaint1_2),
                        QPoint(xpaint, ypaint+ypaint1_2),
                        QPoint(xpaint+xpaint1-1, ypaint+crady)
                    };
                    
                    if(lightB>0) {
                        pnt->setBrush(QColor::fromHsl(col2,satB,lightB));
                    } else {
                        pnt->setBrush(Qt::NoBrush);
                    }
                    pnt->drawPolygon(points2,4);
                }
            } else if (lay->getSegtype(iseg)==0 || lay->getSegtype(iseg)==7) {
                //                pnt->drawRect(xpaint,ypaint,xpaint1,ypaint1);
                pnt->drawRoundedRect(xpaint,ypaint,xpaint1_1,ypaint1_1, cradx, crady);
            } else {
                //                pnt->drawRect(xpaint,ypaint,xpaint1,ypaint1);
                if(xpaint1>ypaint1) {
                    int move=(xpaint1-ypaint1)/2;
                    pnt->drawEllipse(xpaint+move,ypaint,ypaint1_1,ypaint1_1);
                } else {
                    int move=(ypaint1-xpaint1)/2;
                    pnt->drawEllipse(xpaint,ypaint+move,xpaint1_1,xpaint1_1);
                }
            }
            
            if(painttext>0) {
                pnt->setPen(QColor::fromHsl((col+180)%360,100,100));
                pnt->setFont(QFont("Ubuntu",20));
                pnt->drawText(xpaint,ypaint,xpaint1,ypaint1,Qt::AlignCenter,*lay->getSegText(iseg));
            }
            
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
            break;
        case 8:
            colorMode=v; // color mode 0 constant, 1 by note, 2 by segH
            break;
        case 9:
            chue=v;
            break;
        case 10:
            cradx=v;
            break;
        case 11:
            crady=v;
            break;
        case 12:
            gradients=v;
            break;
        case 13:
            painttext=v;
            break;
    }
}
