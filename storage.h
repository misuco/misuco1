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
#ifndef STORAGE_H
#define STORAGE_H

#include <point.h>

class Storage
{
public:
    Storage();
    Point *getPoint(int i);
    void next();
    int getLen();

private:
    Point **pnts;
    int len;
    int put_pnt;
    int np;
};

#endif // STORAGE_H
