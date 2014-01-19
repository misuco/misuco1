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
    float freq_a = 440; // a is 440 hz...
    for (int x = 0; x < 127; ++x)
    {
        midi2f[x] = (freq_a / 32.0) * (pow(2.0 , (((float)x - 9.0)) / 12.0));
        //qDebug() << "note " << x << " f " << midi2f[x];
    }
    /*
     
     "guessed by me"
     01 C      <-  1/1
     02 cis    <- 13/12
     03 D      <-  7/6
     04 dis    <-  5/4
     05 E      <-  4/3
     06 F      <- 17/12
     07 fis    <-  3/2
     08 G      <- 19/12
     09 gis    <-  5/3
     10 A      <-  7/4
     11 Ais    <- 11/6
     12 H      <- 23/12
     13 C      <-  2/1
     
     Reines Interval
     01 C   1/1
     02 des 16/15
     03 D   9/8 bzw. 10/9
     04 es  6/5
     05 E   5/4
     06 F   4/3
     07 fis 45/32
     08 G   3/2
     09 as  8/5
     10 A   5/3
     11 B   16/9 bzw. 9/5
     12 H   15/8
     13 C   2
     
     */
    
    midi2fpure = new double[127];
    int pure_m[] = {1,16,9,6,5,4,45,3,8,5,16,15,2};
    int pure_d[] = {1,15,8,5,4,3,32,2,5,3,9,8,1};
    float freq_c = freq_a / 5 * 3; // 264 hz
    for (int x = 60; x < 72; ++x)
    {
        float freq=freq_c * pure_m[x%12] / pure_d[x%12];
        midi2fpure[x] = freq;
        int xup=x; float frequp=freq;
        int xdown=x; float freqdown=freq;
        for(int y=0;y<6;y++) {
            xup+=12;
            xdown-=12;
            frequp*=2;
            freqdown/=2;
            if(xup<128) {
                midi2fpure[xup] = frequp;
            }
            if(xdown>0) {
                midi2fpure[xdown] = freqdown;
            }
        }
        // qDebug() << "note " << x << " f " << midi2fpure[x];
    }
    
    midi2TextEU = new QString[12];
    midi2TextEU[0]="C";
    midi2TextEU[1]="C#";
    midi2TextEU[2]="D";
    midi2TextEU[3]="D#";
    midi2TextEU[4]="E";
    midi2TextEU[5]="F";
    midi2TextEU[6]="F#";
    midi2TextEU[7]="G";
    midi2TextEU[8]="G#";
    midi2TextEU[9]="A";
    midi2TextEU[10]="A#";
    midi2TextEU[11]="B";
    
    midi2TextIN = new QString[12];
    midi2TextIN[0]="SA";
    midi2TextIN[1]="SA'";
    midi2TextIN[2]="RE";
    midi2TextIN[3]="RE'";
    midi2TextIN[4]="GA";
    midi2TextIN[5]="MA";
    midi2TextIN[6]="MA'";
    midi2TextIN[7]="PA";
    midi2TextIN[8]="PA'";
    midi2TextIN[9]="DA";
    midi2TextIN[10]="DA'";
    midi2TextIN[11]="NI";
    
    midi2TextDO = new QString[12];
    midi2TextDO[0]="DO";
    midi2TextDO[1]="DO";
    midi2TextDO[2]="RE";
    midi2TextDO[3]="RE";
    midi2TextDO[4]="MI";
    midi2TextDO[5]="FA";
    midi2TextDO[6]="FA";
    midi2TextDO[7]="SO";
    midi2TextDO[8]="SO";
    midi2TextDO[9]="LA";
    midi2TextDO[10]="LA";
    midi2TextDO[11]="TI";
    
    basenote=60;
    basescale=0;
    noct=2;
    
    nFactoryScales=10;
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
    factoryScaleValues[15]=2;
    factoryScaleValues[16]=2;
    factoryScaleValues[17]=2;
    factoryScaleValues[18]=1;
    
    factoryScaleStart[2]=20;
    factoryScaleLen[2]=6;
    factoryScaleValues[20]=1;
    factoryScaleValues[21]=2;
    factoryScaleValues[22]=3;
    factoryScaleValues[23]=2;
    factoryScaleValues[24]=3;
    factoryScaleValues[25]=1;
    
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
    /*
     . -----
     . -----
     . xxxxx
     . |
     . |
     . |
     . |
     . |
     . |
     
     */
    // basic config
    nrows=3;
    rowheightmax=9;
    
    nseg[1]=10;
    segwidthmax[1]=10;
    rowheight[1]=2;
    
    nseg[0]=1;
    segwidthmax[0]=1;
    rowheight[0]=1;
    
    // row 1: control basenote
    for(int i=1;i<11;i++) {
        segtype[i]=6;
        midinote[i]=i-1;
        chan[i]=i-1;
    }
    
    // row 2: info bar
    segtype[0]=7;
    midinote[0]=0;
    chan[0]=0;
    
    int calcnote;
    
    switch (p) {
        case 0:
            segText[0]="MISUCO.ORG MICROTONAL SURFACE CTL";
            nsegs=15;
            nseg[2]=4;
            segwidthmax[2]=4;
            rowheight[2]=6;
            
            // row 2: the scale
            calcnote=basenote;
            for(int i=0;i<nseg[2];i++) {
                midinote[i+11]=calcnote;
                note[i+11]=midi2f[calcnote];
                segText[i+11]="";
                segtype[i+11]=0;
                ctlx[i+11]=11;
                ctly[i+11]=12;
                calcnote+=factoryScaleValues[factoryScaleStart[6]+(i%factoryScaleLen[6])];
            }
            segText[11]="THE";
            segText[12]="FUTURE";
            segText[13]="OF";
            segText[14]="SOUND";
            break;
            
        case 1:
            segText[0]="SIMPLE SCALE - ALL MY LITTLE DUCKS";
            nsegs=19;
            nseg[2]=8;
            segwidthmax[2]=8;
            rowheight[2]=6;
            
            // row 2: the scale
            calcnote=basenote;
            for(int i=0;i<nseg[2];i++) {
                midinote[i+11]=calcnote;
                note[i+11]=midi2f[calcnote];
                segText[i+11]=midi2TextDO[calcnote%12];
                segtype[i+11]=0;
                ctlx[i+11]=11;
                ctly[i+11]=12;
                calcnote+=factoryScaleValues[factoryScaleStart[1]+(i%factoryScaleLen[1])];
            }
            break;
            
        case 2:
            segText[0]="THE POWER OF INDIAN RAGAS";
            nsegs=19;
            nseg[2]=8;
            segwidthmax[2]=8;
            rowheight[2]=6;
            
            // row 2: the scale
            calcnote=basenote;
            for(int i=0;i<nseg[2];i++) {
                midinote[i+11]=calcnote;
                note[i+11]=midi2f[calcnote];
                segText[i+11]=midi2TextIN[calcnote%12];
                segtype[i+11]=0;
                ctlx[i+11]=11;
                ctly[i+11]=12;
                calcnote+=factoryScaleValues[factoryScaleStart[2]+(i%factoryScaleLen[2])];
            }
            break;
            
        case 3:
            segText[0]="FULL PIANO SCALE";
            nsegs=22;
            nseg[2]=13;
            segwidthmax[2]=13;
            rowheight[2]=6;
            
            // row 2: the scale
            calcnote=basenote;
            for(int i=0;i<nseg[2];i++) {
                midinote[i+11]=calcnote;
                note[i+11]=midi2f[calcnote];
                segText[i+11]=midi2TextEU[calcnote%12];
                segtype[i+11]=0;
                ctlx[i+11]=11;
                ctly[i+11]=12;
                calcnote+=factoryScaleValues[factoryScaleStart[0]+(i%factoryScaleLen[0])];
            }
            break;
            
        case 4:
            segText[0]="UNIQUE: FREQ TRANSISION AREAS";
            nsegs=18;
            nseg[2]=7;
            segwidthmax[2]=7;
            rowheight[2]=6;
            
            // row 2: the scale
            calcnote=basenote;
            for(int i=0;i<nseg[2];i++) {
                ctly[i+11]=10;
                if(i%2==0) {
                    midinote[i+11]=calcnote;
                    note[i+11]=midi2f[calcnote];
                    segText[i+11]=midi2TextEU[calcnote%12];
                    segtype[i+11]=0;
                    ctlx[i+11]=11;
                    ctly[i+11]=12;
                    calcnote+=factoryScaleValues[factoryScaleStart[3]+((i/2)%factoryScaleLen[3])];
                } else {
                    segtype[i+11]=1;
                    segText[i+11]="";
                    segtype[i+11]=1;
                    ctlx[i+11]=11;
                    ctly[i+11]=12;
                }
            }
            break;
            
        case 5:
            segText[0]="TEMPERED VS. PURE SCALES";
            nrows=4;
            nsegs=37;
            nseg[2]=13;
            nseg[3]=13;
            segwidthmax[2]=13;
            segwidthmax[3]=13;
            rowheight[2]=3;
            rowheight[3]=3;
            
            // row 2: the scale
            calcnote=basenote;
            for(int i=0;i<nseg[2];i++) {
                midinote[i+11]=calcnote;
                note[i+11]=midi2f[calcnote];
                segText[i+11]=midi2TextEU[calcnote%12];
                segtype[i+11]=0;
                ctlx[i+11]=11;
                ctly[i+11]=12;
                
                midinote[i+24]=calcnote;
                note[i+24]=midi2fpure[calcnote];
                segText[i+24]=midi2TextEU[calcnote%12];
                segtype[i+24]=0;
                ctlx[i+24]=11;
                ctly[i+24]=12;
                calcnote+=factoryScaleValues[factoryScaleStart[0]+(i%factoryScaleLen[0])];
            }
            break;
            
        case 6:
            segText[0]="MATRIX";
            nrows=6;
            nsegs=51;
            nseg[2]=10;
            nseg[3]=10;
            nseg[4]=10;
            nseg[5]=10;
            segwidthmax[2]=10;
            segwidthmax[3]=10;
            segwidthmax[4]=10;
            segwidthmax[5]=10;
            rowheightmax=18;
            rowheight[0]=2;
            rowheight[1]=4;
            rowheight[2]=3;
            rowheight[3]=3;
            rowheight[4]=3;
            rowheight[5]=3;
            
            // row 2: the scale
            calcnote=basenote-36;
            for(int i=0;i<nsegs-11;i++) {
                midinote[i+11]=calcnote;
                note[i+11]=midi2f[calcnote];
                segText[i+11]=midi2TextEU[calcnote%12];
                segtype[i+11]=0;
                ctlx[i+11]=11;
                ctly[i+11]=12;
                calcnote+=factoryScaleValues[factoryScaleStart[5]+(i%factoryScaleLen[5])];
            }
            break;
            
        default:
            segText[10]="PLEASE SUPPORT: MISUCO.ORG";
            nsegs=18;
            nseg[2]=7;
            segwidthmax[2]=7;
            rowheight[2]=6;
            
            // row 2: the scale
            calcnote=basenote+p;
            for(int i=0;i<nseg[2];i++) {
                if(i%2==0) {
                    segtype[i+11]=0;
                    midinote[i+11]=calcnote;
                    note[i+11]=midi2f[calcnote];
                    segText[i+11]=midi2TextEU[calcnote%12];
                    ctlx[i+11]=11;
                    ctly[i+11]=12;
                    calcnote+=factoryScaleValues[factoryScaleStart[p]+((i/2)%factoryScaleLen[p])];
                } else {
                    segtype[i+11]=1;
                    ctlx[i+11]=11;
                    ctly[i+11]=12;
                    segText[i+11]="";
                }
            }
            break;
    }
    calcGeo(this->width, this->height);
    
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
            segText[i+40]=midi2TextEU[calcnote%12];
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
