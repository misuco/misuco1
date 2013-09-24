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
#include "point.h"

Point::Point()
{
    x=0;
    y=0;
    width=1;
    height=1;
    xn=0;
    yn=0;
    t=0;
    gid=0;
    ttl=5000;
}

void Point::set(int x, int y, int w, int h)
{
    this->width=w;
    this->height=h;
    setX(x);
    setY(y);
}

void Point::setX(int x)
{
    this->x=x;
    this->xn=(double)x/(double)width;
}

void Point::setY(int y)
{
    this->y=y;
    this->yn=(double)y/(double)height;
}

void Point::setWidth(int w)
{
    if(w==0) {
        w=1;
    }
    width=w;
    this->x=xn*(double)w;
}

void Point::setHeight(int h)
{
    if(h==0) {
        h=1;
    }
    height=h;
    this->y=yn*(double)h;
}

void Point::setT(long t)
{
    this->t=t;
}

void Point::setTTL(long ttl)
{
    this->ttl=ttl;
}

void Point::setGid(int id)
{
    gid = id;
}

void Point::setState(int s)
{
    state=s;

}

int Point::getX()
{
    return x;
}

int Point::getY()
{
    return y;
}

double Point::getXn()
{
    return xn;
}

double Point::getYn()
{
    return yn;
}

long Point::getT()
{
    return t;
}

int Point::getGid()
{
    return gid;
}

int Point::getState()
{
    return state;
}
