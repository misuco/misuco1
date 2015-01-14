#include "paintwave.h"

PaintWave::PaintWave(synth::Controller * c)
{
    ctl=c;
}

void PaintWave::paint(RC1 *rc1, QPainter *pnt)
{
    if(!ctl->released()) {
        // fit the wave into the scale rows
        LayoutModel * l=rc1->getLayout();
        int yv=l->waveDisplayY;
        int hv=l->waveDisplayH;

        // offset to the first positive zero-transition
        pnt->setPen(Qt::white);
        int offset=0;
        while(offset<rc1->width()) {
            offset++;
            if( ctl->getSampleMem(offset-1)<=0 &&
                ctl->getSampleMem(offset)>0 ) break;
        }

        // get the first value
        int yp=ctl->getSampleMem(offset)*hv;
        int y=0;

        for(int i=1;i<rc1->width();i++) {
            y=ctl->getSampleMem(i+offset)*hv;
            pnt->drawLine(i-1,yv-yp,i,yv-y);
            yp=y;
        }
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
