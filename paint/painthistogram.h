#ifndef PAINTHISTOGRAM_H
#define PAINTHISTOGRAM_H

#include "ipaint.h"

class PaintHistogram: public IPaint
{
public:
    PaintHistogram();
    virtual void paint(RC1 *rc1, QPainter *pnt);
    virtual int getParamCount();
    virtual float getParam(quint16 n);
    virtual float getParamMin(quint16 n);
    virtual float getParamMax(quint16 n);
    virtual void setParam(quint16, float);
};

#endif // PAINTHISTOGRAM_H
