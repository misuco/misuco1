#ifndef PAINTBLOCKER_H
#define PAINTBLOCKER_H

#include "ipaint.h"

class PaintBlocker : public IPaint
{
public:
    PaintBlocker();
    virtual void paint(RC1 *view, QPainter *pnt);
    virtual int getParamCount() {return 0;}
    virtual float getParam(quint16)  {return 0;}
    virtual float getParamMin(quint16)  {return 0;}
    virtual float getParamMax(quint16)  {return 0;}
    virtual void setParam(quint16, float) {}
private:
    int closeXpos;
    int closeWidth;
};

#endif // PAINTBLOCKER_H
