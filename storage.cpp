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
#include "storage.h"
#include <QDebug>

Storage::Storage()
{
    len=500;
    pnts = new Point*[len];
    put_pnt=0;
    for(int i=0;i<len;i++) {
        pnts[i]=new Point();
    }
}

void Storage::next()
{
    put_pnt++;
    if(put_pnt>=len) {
        put_pnt=0;
    }
}

Point *Storage::getPoint(int i)
{
    if(i<len) {
        int j=put_pnt-i;
        if(j<0) {
            j+=len;
        }
        return pnts[j];
    } else {
        return NULL;
    }
}

int Storage::getLen()
{
    return len;
}
