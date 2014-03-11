#ifndef PAINTBLOCKER_H
#define PAINTBLOCKER_H

#include "ipaint.h"

class PaintBlocker : public IPaint
{
public:
    PaintBlocker();
    virtual void paint(RC1 *view, QPainter *pnt);
    virtual int getParamCount() {return 0;}
    virtual float getParam(quint16 n)  {return 0;}
    virtual float getParamMin(quint16 n)  {return 0;}
    virtual float getParamMax(quint16 n)  {return 0;}
    virtual void setParam(quint16 n, float v) {}

};

#endif // PAINTBLOCKER_H
