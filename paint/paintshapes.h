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
#ifndef PAINERSHAPES_H
#define PAINERSHAPES_H

#include "ipaint.h"
#include  "pointpaintshape.h"

class PaintShapes : public IPaint
{
public:
    PaintShapes();
    virtual void paint(RC1 *view, QPainter *pnt);
    virtual int getParamCount();
    virtual float getParam(quint16 n);
    virtual float getParamMin(quint16 n);
    virtual float getParamMax(quint16 n);
    virtual void setParam(quint16 n, float v);

private:
    PointPaintShape * pps;
    float * params;
    quint16 nparams;
    quint16 ncols;
    quint16 nrows;
};

#endif // PAINERSHAPES_H
