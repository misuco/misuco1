#include "paintwave.h"

PaintWave::PaintWave(synth::Controller * c)
{
    ctl=c;
}

void PaintWave::paint(RC1 *rc1, QPainter *pnt)
{
    LayoutModel * l=rc1->getLayout();
    int yv=l->waveDisplayY;
    int hv=l->waveDisplayH;
    if(!ctl->released()) {
        pnt->setPen(Qt::lightGray);
        // fit the wave into the scale rows

        // offset to the first positive zero-transition
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
    pnt->setPen(Qt::black);
    QString fps,freqs;
    if(rc1->sequencer->isManuStop()) {
        fps.sprintf("*");
    } else {
        fps.sprintf(" ");
    }
    if(rc1->sequencer->isSyncMaster()) {
        fps.append(freqs.sprintf("M "));
    } else {
        fps.append(freqs.sprintf("S "));
    }
    fps.append(freqs.sprintf("%d step %d fps %d/%d voices ",rc1->sequencer->getStep(),rc1->getFps(),ctl->getNVoices(),ctl->getNVoicesMax()));
//    fps.sprintf(" %d fps %d/%d voices ",1,ctl->getNVoices(),ctl->getNVoicesMax());
    for(int i=ctl->getNVoices()-1;i>=0;i--) {
        fps.append(freqs.sprintf("  %.1f",ctl->getVF(i)));
    }
    pnt->setFont(QFont(l->getFont(),l->getFontsizeS()));
    pnt->drawText(0,rc1->height()-l->getFontsize(),rc1->width(),hv,Qt::AlignLeft,fps);

}

int PaintWave::getParamCount() {
    return 0;
}

float PaintWave::getParam(quint16) {
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
