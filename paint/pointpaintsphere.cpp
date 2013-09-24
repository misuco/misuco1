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
#include "pointpaintsphere.h"

PointPaintSphere::PointPaintSphere()
{
}

void PointPaintSphere::paint(Point *point, RC1 *v, QPainter *pnt)
{
    int rad=v->getLayout()->getWidth()*(v->getNow()-point->getT())/v->getTtl();
//    qDebug() << v->getNow() << " " << point->getT() << " " << rad;
    pnt->setBrush(Qt::NoBrush);
    pnt->setPen(Qt::white);
    pnt->drawEllipse(point->getX()-rad,point->getY()-rad, rad*2, rad*2);
}

int PointPaintSphere::getParamCount()
{
    return 0;
}

float PointPaintSphere::getParam(quint16 n)
{
    return 0;
}

float PointPaintSphere::getParamMin(quint16 n)
{
    return 0;
}

float PointPaintSphere::getParamMax(quint16 n)
{
    return 0;
}

void PointPaintSphere::setParam(quint16 n, float v)
{

}
