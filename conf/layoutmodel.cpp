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
#include <math.h>
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
    setAll(nsegsmax,segwidth,1);
    segwidthpx = new int[nsegsmax];
    segwidthmax=new int[nrowsmax];
    note = new double[nsegsmax];
    midinote = new int[nsegsmax];
    segText=new QString[nsegsmax];
    segH = new int[nsegsmax];
    // put numbers as text
    for(int i=0;i<nsegsmax;i++) {
        segText[i].setNum(i);
        segH[i]=i*10%255;
    }
    segtype = new int[nsegsmax];
    setAll(nsegsmax,segtype,0);

    ctlx=new int[nsegsmax];
    setAll(nsegsmax,ctlx,16);

    ctly=new int[nsegsmax];
    setAll(nsegsmax,ctly,8);

    chan=new int[nsegsmax];
    setAll(nsegsmax,chan,0);

    pressed=new int[nsegsmax];
    setAll(nsegsmax,pressed,0);

    // thanx 2 http://subsynth.sourceforge.net/midinote2freq.html
    midi2f = new double[127];
    float a = 440; // a is 440 hz...
    for (int x = 0; x < 127; ++x)
    {
       midi2f[x] = (a / 32.0) * (pow(2.0 , (((float)x - 9.0)) / 12.0));
       //qDebug() << "note " << x << " f " << midi2f[x];
    }

    midi2Text = new QString[12];
    midi2Text[0]="SA\nC";
    midi2Text[1]="SA'\nC#";
    midi2Text[2]="RE\nD";
    midi2Text[3]="RE'\nD#";
    midi2Text[4]="GA\nE";
    midi2Text[5]="MA\nF";
    midi2Text[6]="MA'\nF#";
    midi2Text[7]="PA\nG";
    midi2Text[8]="PA'\nG#";
    midi2Text[9]="DA\nA";
    midi2Text[10]="DA'\nA#";
    midi2Text[11]="NI\nB";

    basenote=60;
    basescale=0;
    noct=2;

    nFactoryScales=8;
    factoryScaleStart=new int[nFactoryScales];
    factoryScaleLen=new int[nFactoryScales];
    factoryScaleValues=new int[56];

    factoryScaleStart[0]=0;
    factoryScaleLen[0]=12;
    factoryScaleValues[0]=1;
    factoryScaleValues[1]=1;
    factoryScaleValues[2]=1;
    factoryScaleValues[3]=1;
    factoryScaleValues[4]=1;
    factoryScaleValues[5]=1;
    factoryScaleValues[6]=1;
    factoryScaleValues[7]=1;
    factoryScaleValues[8]=1;
    factoryScaleValues[9]=1;
    factoryScaleValues[10]=1;
    factoryScaleValues[11]=1;

    factoryScaleStart[1]=12;
    factoryScaleLen[1]=7;
    factoryScaleValues[12]=2;
    factoryScaleValues[13]=2;
    factoryScaleValues[14]=1;
    factoryScaleValues[16]=2;
    factoryScaleValues[17]=2;
    factoryScaleValues[18]=2;
    factoryScaleValues[19]=1;

    factoryScaleStart[2]=20;
    factoryScaleLen[2]=7;
    factoryScaleValues[20]=2;
    factoryScaleValues[21]=1;
    factoryScaleValues[22]=2;
    factoryScaleValues[23]=2;
    factoryScaleValues[24]=2;
    factoryScaleValues[25]=2;
    factoryScaleValues[26]=1;

    factoryScaleStart[3]=27;
    factoryScaleLen[3]=5;
    factoryScaleValues[27]=2;
    factoryScaleValues[28]=3;
    factoryScaleValues[29]=2;
    factoryScaleValues[30]=4;
    factoryScaleValues[31]=1;

    factoryScaleStart[4]=32;
    factoryScaleLen[4]=5;
    factoryScaleValues[32]=3;
    factoryScaleValues[33]=2;
    factoryScaleValues[34]=3;
    factoryScaleValues[35]=2;
    factoryScaleValues[36]=2;

    factoryScaleStart[5]=37;
    factoryScaleLen[5]=5;
    factoryScaleValues[37]=3;
    factoryScaleValues[38]=3;
    factoryScaleValues[39]=3;
    factoryScaleValues[40]=1;
    factoryScaleValues[41]=2;

    factoryScaleStart[6]=42;
    factoryScaleLen[6]=3;
    factoryScaleValues[42]=4;
    factoryScaleValues[43]=3;
    factoryScaleValues[44]=5;
    
    factoryScaleStart[7]=45;
    factoryScaleLen[7]=3;
    factoryScaleValues[45]=3;
    factoryScaleValues[46]=4;
    factoryScaleValues[47]=5;
    
    factoryScaleStart[8]=48;
    factoryScaleLen[8]=4;
    factoryScaleValues[48]=3;
    factoryScaleValues[49]=4;
    factoryScaleValues[50]=3;
    factoryScaleValues[51]=2;
    
    factoryScaleStart[9]=52;
    factoryScaleLen[9]=4;
    factoryScaleValues[52]=3;
    factoryScaleValues[53]=3;
    factoryScaleValues[54]=4;
    factoryScaleValues[55]=2;

    setFactoryProg(0);

//    setScale(36,127,0,false);
//    setXY(8,4);
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

int LayoutModel::getNsegs() const
{
    return nsegs;
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

double LayoutModel::getNote(int i) const
{
    return note[i];
}

int LayoutModel::getMidiNote(int i) const
{
    return midinote[i];
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

int LayoutModel::getSegtype(int i) const
{
    return segtype[i];
}

QString *LayoutModel::getSegText(int i) const
{
    return &(segText[i]);
}

int LayoutModel::getSegH(int i) const
{
    return segH[i];
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

void LayoutModel::setRaga(int i, int b)
{
    /*
     * Raag Bihag
     *
     * ni sa ga ma pa ni sa
     * -1 1  4  1  2  4  1
     * 59 60 64 65 67 71 72
     * (7 notes + 6 trans = 13 segs)
     *
     * sa re ga ma pa da ni sa
     * 60 62 64 65 67 69 71 72
     * (8 notes + 7 trans = 15 segs)
     *
     * ni sa ga ma pa ni sa
     * 59 60 64 65 67 71 72
     * (7 notes + 6 trans = 13 segs)
     *
     * sa ni sa re ga ma ga pa ma pa da ni sa
     * 60 59 60 62 64 65 64 67 66 67 69 71 72
     * (13 notes + 12 trans = 25 segs)
     *
     **/

    // basic config
    nrows=4;
    rowheightmax=nrows;
    nseg[0]=13;
    segwidthmax[0]=13;
    rowheight[0]=1;
    nseg[1]=15;
    segwidthmax[1]=15;
    rowheight[1]=1;
    nseg[2]=13;
    segwidthmax[2]=13;
    rowheight[2]=1;
    nseg[3]=25;
    segwidthmax[3]=25;
    rowheight[3]=1;
    nsegs=13+15+13+25; // =66
    setAll(nsegs,segwidth,1);
    calcGeo(width,height);

    // row 1
    segtype[0]=0;
    segtype[1]=1;
    segtype[2]=0;
    segtype[3]=1;
    segtype[4]=0;
    segtype[5]=1;
    segtype[6]=0;
    segtype[7]=1;
    segtype[8]=0;
    segtype[9]=1;
    segtype[10]=0;
    segtype[11]=1;
    segtype[12]=0;
    note[0]=midi2f[59];
    note[2]=midi2f[60];
    note[4]=midi2f[64];
    note[6]=midi2f[65];
    note[8]=midi2f[67];
    note[10]=midi2f[71];
    note[12]=midi2f[72];
    midinote[0]=59;
    midinote[2]=60;
    midinote[4]=64;
    midinote[6]=65;
    midinote[8]=67;
    midinote[10]=71;
    midinote[12]=72;

    // row 2
    segtype[13]=0;
    segtype[14]=1;
    segtype[15]=0;
    segtype[16]=1;
    segtype[17]=0;
    segtype[18]=1;
    segtype[19]=0;
    segtype[20]=1;
    segtype[21]=0;
    segtype[22]=1;
    segtype[23]=0;
    segtype[24]=1;
    segtype[25]=0;
    segtype[26]=1;
    segtype[27]=0;
    note[13]=midi2f[60];
    note[15]=midi2f[62];
    note[17]=midi2f[64];
    note[19]=midi2f[65];
    note[21]=midi2f[67];
    note[23]=midi2f[69];
    note[25]=midi2f[71];
    note[27]=midi2f[72];
    midinote[13]=60;
    midinote[15]=62;
    midinote[17]=64;
    midinote[19]=65;
    midinote[21]=67;
    midinote[23]=69;
    midinote[25]=71;
    midinote[27]=72;

    // row 3
    segtype[28]=0;
    segtype[29]=1;
    segtype[30]=0;
    segtype[31]=1;
    segtype[32]=0;
    segtype[33]=1;
    segtype[34]=0;
    segtype[35]=1;
    segtype[36]=0;
    segtype[37]=1;
    segtype[38]=0;
    segtype[39]=1;
    segtype[40]=0;
    note[28]=midi2f[59];
    note[30]=midi2f[60];
    note[32]=midi2f[64];
    note[34]=midi2f[65];
    note[36]=midi2f[67];
    note[38]=midi2f[71];
    note[40]=midi2f[72];
    midinote[28]=59;
    midinote[30]=60;
    midinote[32]=64;
    midinote[34]=65;
    midinote[36]=67;
    midinote[38]=71;
    midinote[40]=72;

    // row 4
    segtype[41]=0;
    segtype[42]=1;
    segtype[43]=0;
    segtype[44]=1;
    segtype[45]=0;
    segtype[46]=1;
    segtype[47]=0;
    segtype[48]=1;
    segtype[49]=0;
    segtype[50]=1;
    segtype[51]=0;
    segtype[52]=1;
    segtype[53]=0;
    segtype[54]=1;
    segtype[55]=0;
    segtype[56]=1;
    segtype[57]=0;
    segtype[58]=1;
    segtype[59]=0;
    segtype[60]=1;
    segtype[61]=0;
    segtype[62]=1;
    segtype[63]=0;
    segtype[64]=1;
    segtype[65]=0;
    note[41]=midi2f[60];
    note[43]=midi2f[59];
    note[45]=midi2f[60];
    note[47]=midi2f[62];
    note[49]=midi2f[64];
    note[51]=midi2f[65];
    note[53]=midi2f[64];
    note[55]=midi2f[67];
    note[57]=midi2f[66];
    note[59]=midi2f[67];
    note[61]=midi2f[69];
    note[63]=midi2f[71];
    note[65]=midi2f[72];
    midinote[41]=60;
    midinote[43]=59;
    midinote[45]=60;
    midinote[47]=62;
    midinote[49]=64;
    midinote[51]=65;
    midinote[53]=64;
    midinote[55]=67;
    midinote[57]=66;
    midinote[59]=67;
    midinote[61]=69;
    midinote[63]=71;
    midinote[65]=72;

}
int LayoutModel::getBasenote() const
{
    return basenote;
}

void LayoutModel::setBasenote(int value)
{
    basenote = value;
    setFactoryLayout(0);
}
int LayoutModel::getBasescale() const
{
    return basescale;
}

void LayoutModel::setBasescale(int value)
{
    basescale = value;
    setFactoryLayout(0);
}
int LayoutModel::getNoct() const
{
    return noct;
}

void LayoutModel::setNoct(int value)
{
    noct = value;
    setFactoryLayout(0);
}

void LayoutModel::setScale(int start, int n, int step, bool withTransistion = false)
{
    int nsteps;
    int * steps;
    steps=new int[12];
    if(n<nsegsmax) {
        switch (step) {
        // regular keyboard
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
        int notex1=start;
        int notex=start;
        for(int i=0;i<n;i++) {
            notex=notex1+steps[i%nsteps];
            if(withTransistion) {
                note[i*2]=midi2f[notex];
                segtype[i*2]=0;
                if(i<n-1) {
                    segtype[i*2+1]=1;
                }
            } else {
                note[i]=midi2f[notex];
                segtype[i]=0;
            }
            notex1=notex;
        }
    }
}

void LayoutModel::setFactoryProg(int p)
{
    // basic config
    nsegs=19;
    nrows=2;
    rowheightmax=4;
    nseg[0]=10;
    segwidthmax[0]=10;
    rowheight[0]=1;
    nseg[1]=9;
    segwidthmax[1]=9;
    rowheight[1]=3;
    
    // row 1: control basenote
    for(int i=0;i<10;i++) {
        segtype[i]=6;
        midinote[i]=i;
        chan[i]=i;
    }
    
    // row 2: the scale
    int calcnote=basenote;
    for(int i=0;i<nseg[1];i++) {
        if(i%2==0) {
            midinote[i+10]=calcnote;
            note[i+10]=midi2f[calcnote];
            segText[i+10]=midi2Text[calcnote%12];
            calcnote+=factoryScaleValues[factoryScaleStart[p]+((i/2)%factoryScaleLen[p])];
        } else {
            segtype[i+10]=1;
            segText[i+10]="";
        }
    }
}

void LayoutModel::setFactoryLayout(int i)
{
    // basic config
    nrows=4;
    rowheightmax=12;
    nseg[0]=24;
    segwidthmax[0]=8;
    rowheight[0]=1;
    nseg[1]=8;
    segwidthmax[1]=8;
    rowheight[1]=1;
    nseg[2]=8;
    segwidthmax[2]=8;
    rowheight[2]=1;

    nseg[3]=factoryScaleLen[basescale]*noct*2-1;
    if(nseg[3]+40>nsegsmax) {
        nseg[3]=nsegsmax-40;
    }
    segwidthmax[3]=nseg[3];
    rowheight[3]=9;
    nsegs=40+nseg[3];

    qDebug() << "nsegs " << nsegs;
    setAll(nsegs,segwidth,1);
    qDebug() << "calc geo " << width << " " << height;
    calcGeo(width,height);
    qDebug() << "set segtype 0";
    setAll(nsegs,segtype,0);
    qDebug() << "set chan 0";
    setAll(nsegs,chan,0);


    // row 1: control basenote
    for(int i=0;i<24;i++) {
        qDebug() << "row 1: " << i;
        segtype[i]=3;
        midinote[i]=i+60;
    }

    // row 2-3: control rows
    for(int i=0;i<8;i++) {
        qDebug() << "row 2/3: " << i;
        segtype[i+24]=4;
        segtype[i+32]=5;
        midinote[i+24]=i;
        midinote[i+32]=i;
        chan[i+24]=i;
        chan[i+32]=i+1;
        segText[i+24].sprintf("X%2d",i);
        segText[i+32].sprintf("Y%2d",i);
    }

    // row 4: the scale
    int calcnote=basenote;
    for(int i=0;i<nseg[3];i++) {
        qDebug() << "row 4: " << i;
        if(i%2==0) {
            qDebug() << "calcnote " << calcnote;
            midinote[i+40]=calcnote;
            note[i+40]=midi2f[calcnote];
            segText[i+40]=midi2Text[calcnote%12];
            calcnote+=factoryScaleValues[factoryScaleStart[basescale]+((i/2)%factoryScaleLen[basescale])];
        } else {
            segtype[i+40]=1;
            segText[i+40]="";
        }
    }
}

void LayoutModel::setSegH(int i, int v)
{
    segH[i]=v;
}

void LayoutModel::setAllCtlx(int v)
{
    setAll(nsegs,ctlx,v);
}

void LayoutModel::setAllCtly(int v)
{
    setAll(nsegs,ctly,v);
}
