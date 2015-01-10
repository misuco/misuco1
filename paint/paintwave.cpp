#include "paintwave.h"

PaintWave::PaintWave(synth::Controller * c)
{
    ctl=c;
}

void PaintWave::paint(RC1 *rc1, QPainter *pnt)
{
    float h2=rc1->height()/2;
    float h4=h2/2;
    int yp=ctl->getSampleMem(0)*h4;
    int y=0;
    pnt->setPen(Qt::white);
    for(int i=1;i<rc1->width();i++) {
        y=ctl->getSampleMem(i)*h4;
        pnt->drawLine(i-1,yp+h2,i,y+h2);
        yp=y;
    }
}

int PaintWave::getParamCount() {
    return 0;
}

float PaintWave::getParam(quint16 n) {
    return 0;
}

float PaintWave::getParamMin(quint16) {
    return 0;
}

float PaintWave::getParamMax(quint16) {
    return 0;
}

void PaintWave::setParam(quint16, float) {
}
