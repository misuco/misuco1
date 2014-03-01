#ifndef PAINTBGBITMAP_H
#define PAINTBGBITMAP_H

#include "ipaint.h"

class PaintBgBitmap : public IPaint
{
public:
    PaintBgBitmap();
    virtual void paint(RC1 *view, QPainter *pnt);
    virtual int getParamCount();
    virtual float getParam(quint16 n);
    virtual float getParamMin(quint16 n);
    virtual float getParamMax(quint16 n);
    virtual void setParam(quint16 n, float v);
};

#endif // PAINTBGBITMAP_H
