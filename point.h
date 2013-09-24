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
#ifndef POINT_H
#define POINT_H

class Point
{
public:
    Point();

    void set(int x, int y, int w, int h);
    void setX(int x);
    void setY(int y);
    void setWidth(int w);
    void setHeight(int h);
    void setT(long t);
    void setTTL(long ttl);
    void setGid(int id);
    void setState(int s);

    int getX();
    int getY();
    double getXn();
    double getYn();
    long getT();
    int getGid();
    int getState();

private:
    int x;
    int y;
    double xn;
    double yn;
    int width;
    int height;
    long t;
    long ttl;
    int gid;
    int state;
};

#endif // POINT_H
