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
#ifndef POINTPAINTSHAPE_H
#define POINTPAINTSHAPE_H

#include "ipointpaint.h"

class PointPaintShape : public IPointPaint
{
public:
    PointPaintShape();
    virtual void paint(Point *point, RC1 *v, QPainter *pnt);
    virtual int getParamCount();
    virtual float getParam(quint16 n);
    virtual float getParamMin(quint16 n);
    virtual float getParamMax(quint16 n);
    virtual void setParam(quint16 n, float v);

private:
    float * params;
    quint16 nparams;

    float calcRow(quint16 row, Point *pnt, RC1 *v);
};

#endif // POINTPAINTSHAPE_H
