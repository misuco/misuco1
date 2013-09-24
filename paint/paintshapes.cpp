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
#include "paintshapes.h"

PaintShapes::PaintShapes()
{
    nparams=1;
    params=new float[nparams];
    params[0]=5000.0;
    pps=new PointPaintShape();
}

void PaintShapes::paint(RC1 *view, QPainter *pnt)
{
    pnt->setBrush(Qt::green);
    Storage * storage = view->getStorage();
    for(int i=0; i<storage->getLen();i++) {
        Point * p = storage->getPoint(i);
        if(p!=NULL) {
            if(view->getNow()-p->getT() < params[0]) {
                if(p->getX()>=0 && p->getY()>=0) {
                    pps->paint(p,view,pnt);
                }
            }
        }
    }
}

int PaintShapes::getParamCount() {
    return nparams;  // 8*9 parameters
}

float PaintShapes::getParam(quint16 n) {
    if(n<nparams) {
        return params[n];
    }
}

float PaintShapes::getParamMin(quint16 n) {
    return 0;
}

float PaintShapes::getParamMax(quint16 n) {
    return 0;
}

void PaintShapes::setParam(quint16 n, float v) {
    if(n<nparams) {
        params[n]=v;
    }
}
