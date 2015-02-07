#include "painthistogram.h"

PaintHistogram::PaintHistogram()
{

}

void PaintHistogram::paint(RC1 *rc1, QPainter *pnt)
{
    QString fps;
    pnt->setPen(Qt::red);
    int yp=0;
    int maxat=0;
    int max=0;
    for(int i=0;i<256;i++) {
        int y=rc1->touchstat.getLatSum(i);
        pnt->drawLine(i,yp,i+1,y);
        if(y>max) {
            max=y;
            maxat=i;
        }
        yp=y;
    }
    pnt->drawLine(0,max,rc1->width(),max);
    fps.sprintf("max %d at %d ms ",max,maxat);
    pnt->drawText(maxat,max,300,100,Qt::AlignLeft,fps);

    pnt->setPen(Qt::blue);
    yp=0;
    maxat=0;
    max=0;
    for(int i=0;i<256;i++) {
        int y=rc1->touchstat.getFpsSum(i);
        pnt->drawLine(i,yp,i+1,y);
        if(y>max) {
            max=y;
            maxat=i;
        }
        yp=y;
    }
    pnt->drawLine(0,max,rc1->width(),max);
    fps.sprintf("max %d at %d ms ",max,maxat);
    pnt->drawText(maxat,max,300,100,Qt::AlignLeft,fps);
}

int PaintHistogram::getParamCount() {
    return 0;
}

float PaintHistogram::getParam(quint16) {
    return 0;
}

float PaintHistogram::getParamMin(quint16) {
    return 0;
}

float PaintHistogram::getParamMax(quint16) {
    return 0;
}

void PaintHistogram::setParam(quint16, float) {
}

