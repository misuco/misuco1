#ifndef PAINTWAVE_H
#define PAINTWAVE_H

#include "ipaint.h"
#include <mobilesynth/mobilesynth/Classes/synth/controller.h>

class PaintWave : public IPaint
{
public:
    PaintWave(synth::Controller *c);
    virtual void paint(RC1 *rc1, QPainter *pnt);
    virtual int getParamCount();
    virtual float getParam(quint16 n);
    virtual float getParamMin(quint16 n);
    virtual float getParamMax(quint16 n);
    virtual void setParam(quint16, float);
    virtual void setSynthController(synth::Controller * c) {
        ctl=c;
    }

private:
    synth::Controller * ctl;
};

#endif // PAINTWAVE_H
