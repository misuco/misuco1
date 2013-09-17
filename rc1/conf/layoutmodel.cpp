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
#include <QDebug>
#include "layoutmodel.h"

LayoutModel::LayoutModel()
{
    width=200;
    height=200;
    nrowsmax=16;
    nsegsmax=16*16;
    nseg = new int[nrowsmax];
    rowheight = new int[nrowsmax];
    rowheightpx = new int[nrowsmax];
    segwidth = new int[nsegsmax];
    segwidthpx = new int[nsegsmax];
    segwidthmax=new int[nrowsmax];
    note = new int[nsegsmax];
    segText=new QString[nsegsmax];
    for(int i=0;i<nsegsmax;i++) {
        segText[i].setNum(i);
    }

    ctlx=new int[nsegsmax];
    setAll(nsegs,ctlx,0);

    ctly=new int[nsegsmax];
    setAll(nsegs,ctly,0);

    chan=new int[nsegsmax];
    setAll(nsegs,chan,0);

    pressed=new int[nsegsmax];
    setAll(nsegs,pressed,0);

    setScale(0,127,0);
    setXY(4,4);
}

void LayoutModel::calcGeo(int w, int h)
{
    // qDebug() << "Cacl geo " << w << " " << h;
    width=w;
    height=h;
    int i=0;
    int rowheightsum=0;
    for(int y=0;y<nrows;y++) {
        rowheightpx[y]=height*rowheight[y]/rowheightmax;
        rowheightsum+=rowheightpx[y];
        // additional pixels may occur due to rounding differences
        // -> add additional pixels to last row
        if(y==nrows-1 && rowheightsum<height) {
            rowheightpx[y]+=rowheightsum-height;
        }
        int segwidthsum=0;
        for(int x=0;x<nseg[y];x++) {
            segwidthpx[i]=width*segwidth[i]/segwidthmax[y];
            segwidthsum+=segwidthpx[i];
            // -> add additional pixels to last segment
            if(x==nseg[y]-1 && segwidthsum<width) {
                segwidthpx[i]+=width-segwidthsum;
            }
            i++;
        }
    }
}

int LayoutModel::getHeight() const
{
    return height;
}

int LayoutModel::getWidth() const
{
    return width;
}

int LayoutModel::getNrows() const
{
    return nrows;
}

int LayoutModel::getRowheightpx(int i) const
{
    return rowheightpx[i];
}

int LayoutModel::getNseg(int i) const
{
    return nseg[i];
}

void LayoutModel::setAll(int n, int *d, int v)
{
    for(int i=0;i<n;i++) {
        d[i]=v;
    }
}

int LayoutModel::getSegwidth(int i) const
{
    return segwidth[i];
}

int LayoutModel::getCtly(int i) const
{
    return ctly[i];
}

int LayoutModel::getChan(int i) const
{
    return chan[i];
}

int LayoutModel::getCtlx(int i) const
{
    return ctlx[i];
}

int LayoutModel::getNote(int i) const
{
    return note[i];
}


int LayoutModel::getSegwidthpx(int i) const
{
    return segwidthpx[i];
}


int LayoutModel::getSegwidthmax(int i) const
{
    return segwidthmax[i];
}

int LayoutModel::getPressed(int i) const
{
    return pressed[i];
}

QString *LayoutModel::getSegText(int i) const
{
    return &(segText[i]);
}

void LayoutModel::incPressed(int i)
{
    pressed[i]++;
}

void LayoutModel::decPressed(int i)
{
    if(pressed[i]>0) {
        pressed[i]--;
    }
}

void LayoutModel::setXY(int x, int y)
{
    if(y<=nrowsmax && x*y<=nsegsmax) {
        nrows=y;
        nsegs=x*y;
        rowheightmax=nrows;
        for(int i=0;i<y;i++) {
            nseg[i]=x;
            segwidthmax[i]=x;
            rowheight[i]=1;
        }
        setAll(nsegs,segwidth,1);
        calcGeo(width,height);
    }
}

void LayoutModel::setScale(int start, int n, int step)
{
    int nsteps;
    int * steps;
    steps=new int[12];
    if(n<nsegsmax) {
        switch (step) {
        case 1:
            steps[0]=2;
            steps[1]=2;
            steps[2]=1;
            steps[3]=2;
            steps[4]=2;
            steps[5]=2;
            steps[6]=1;
            nsteps=7;
            break;
        case 2:
            steps[0]=2;
            steps[1]=1;
            steps[2]=2;
            steps[3]=2;
            steps[4]=2;
            steps[5]=2;
            steps[6]=1;
            nsteps=7;
            break;
        case 3:
            steps[0]=4;
            steps[1]=3;
            steps[2]=5;
            nsteps=3;
            break;
        default:
            steps[0]=1;
            nsteps=1;
            break;
        }
        if(step==0) {
            steps[0] = 1;
        }
        note[0]=start;
        for(int i=1;i<n;i++) {
            note[i]=note[i-1]+steps[i%nsteps];
        }
    }
}

