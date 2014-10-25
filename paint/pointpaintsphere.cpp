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
    if(point->getHue()>=0) {
        float age=(float)(v->getNow()-point->getT())/(float)v->getTtl();
        int rad=(float)v->getLayout()->getWidth()/2*age;
        //    qDebug() << v->getNow() << " " << point->getT() << " " << rad;
        pnt->setBrush(Qt::NoBrush);
        pnt->setPen(QColor::fromHsl(point->getHue(), 230, 200-200.0*age ));
        pnt->drawEllipse(point->getX()-rad/2,point->getY()-rad/2, rad, rad);
//        pnt->drawRoundedRect(point->getX()-rad/2,point->getY()-rad/2, rad, rad, 10 , 10);
    }
}

int PointPaintSphere::getParamCount()
{
    return 0;
}

float PointPaintSphere::getParam(quint16)
{
    return 0;
}

float PointPaintSphere::getParamMin(quint16)
{
    return 0;
}

float PointPaintSphere::getParamMax(quint16)
{
    return 0;
}

void PointPaintSphere::setParam(quint16, float)
{

}
