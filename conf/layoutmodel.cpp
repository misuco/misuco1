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
    width=1;
    height=1;
    fontsize=1;
    font="Arial";
    nrowsmax=16;
    nsegsmax=16*16;
    nseg = new int[nrowsmax];
    rowheight = new int[nrowsmax];
    rowheightpx = new int[nrowsmax];
    segwidth = new int[nsegsmax];
    setAll(nsegsmax,segwidth,1);
    segwidthpx = new int[nsegsmax];
    segwidthmax=new int[nrowsmax];
    value = new double[nsegsmax];
    valueint = new int[nsegsmax];
    segText=new QString[nsegsmax];
    segH = new int[nsegsmax];
    // put numbers as text
    for(int i=0;i<nsegsmax;i++) {
        segText[i].setNum(i);
        segH[i]=i*10%255;
    }
    for(int i=0;i<11;i++) {
        bscale[i]=false;
    }
    /*
    bscale[4]=true;
    bscale[7]=true;
    */

    segtype = new int[nsegsmax];
    setAll(nsegsmax,segtype,0);
    
    ctlx=new int[nsegsmax];
    setAll(nsegsmax,ctlx,1);
    
    ctly=new int[nsegsmax];
    setAll(nsegsmax,ctly,2);
    
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
        qDebug() << "note " << x << " f " << midi2f[x];
    }

    float freq_c5 = freq_a / 5 * 3; // 264.00 hz @ 60
    float freq_c4 = freq_c5 / 2;    // 132.00 hz @ 48
    float freq_c3 = freq_c4 / 2;    //  66.00 hz @ 36
    float freq_c2 = freq_c3 / 2;    //  33.00 hz @ 24
    float freq_c1 = freq_c2 / 2;    //  17.50 hz @ 12
    float freq_c0 = freq_c1 / 2;    //   8.75 hz @ 12

    midi2fpure = new double[132];
    int pure_m[] = {1,16,9,6,5,4,45,3,8,5,16,15,2};
    int pure_d[] = {1,15,8,5,4,3,32,2,5,3,9,8,1};

    int x=0;
    for(int i=0;i<10;i++) {
        for(int j=0;j<12;j++) {
            midi2fpure[x] = freq_c0 * pure_m[j] / pure_d[j];
            x++;
        }
        freq_c0*=2;
    }
    
    midi2TextEU = new QString[12];
    midi2TextEU[0]="C";
    midi2TextEU[1]="c";
    midi2TextEU[2]="D";
    midi2TextEU[3]="d";
    midi2TextEU[4]="E";
    midi2TextEU[5]="F";
    midi2TextEU[6]="f";
    midi2TextEU[7]="G";
    midi2TextEU[8]="g";
    midi2TextEU[9]="A";
    midi2TextEU[10]="a";
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
    
    basenote=0;
    topoct=6;
    baseoct=3;
    bscaleStartSeg=0;
    bscaleRow=0;
    scaleStartSeg=25;
    scaleRow=4;
    transMode=false;

    //define one simple segment
    rowheightmax=1;
    nrows=1;
    nsegs=1;
    nseg[0]=1;
    rowheight[0]=1;
    segwidthmax[0]=1;
    segwidth[0]=1;
    segtype[0]=0;
    ctlx[0]=1;
    ctly[0]=2;
    segH[0]=100;
    calcGeo(200,200);
    updateLayout();
}

void LayoutModel::calcGeo(int w, int h)
{
    /*
    bool aspect_change=false;
    if(height!=h) {
        aspect_change=true;
    }
    */
    // qDebug() << "Cacl geo " << w << " " << h << " " << width << " " << height;
    width=w;
    height=h;
    int i=0;
    int rowheightsum=0;
    for(int y=0;y<nrows;y++) {
        // if(aspect_change) {
            rowheightpx[y]=height*rowheight[y]/rowheightmax;
            rowheightsum+=rowheightpx[y];
            // additional pixels may occur due to rounding differences
            // -> add additional pixels to last row
            if(y==nrows-1 && rowheightsum<height) {
                rowheightpx[y]+=rowheightsum-height;
            }
        // }
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
    fontsize=height/nrows/5;
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

double LayoutModel::getValue(int i) const
{
    return value[i];
}

int LayoutModel::getValueInt(int i) const
{
    return valueint[i];
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

void LayoutModel::setValue(int i, double v) const
{
    value[i]=v;
}

void LayoutModel::setValueInt(int i, int v) const
{
    valueint[i]=v;
}

int LayoutModel::getBasenote() const
{
    return basenote;
}

void LayoutModel::setBasenote(int value)
{
    pressed[12+basenote]=0;
    basenote = value;
    pressed[12+basenote]=1;
}

bool LayoutModel::getBscale(int n)
{
    return bscale[n];
}

void LayoutModel::setBscale(int n, bool value)
{
    bscale[n]=value;
    pressed[n+bscaleStartSeg]=value;
}

int LayoutModel::getTopoct() const
{
    return topoct;
}

void LayoutModel::setTopoct(int v)
{
    topoct = v;
//    valueint[24] = v;
//    value[24] = 0.1f*(float)v;
}

int LayoutModel::getBaseoct() const
{
    return baseoct;
}

void LayoutModel::setBaseoct(int v)
{
    baseoct=v;
//    valueint[23] = v;
//    value[23] = 0.1f*(float)v;
}

void LayoutModel::setTransMode(bool t)
{
    transMode=t;
}

void LayoutModel::setRowheightpx(int i, int v)
{
    rowheightpx[i]=v;
}

void LayoutModel::setRowheight(int i, int v)
{
    rowheight[i]=v;
}

int LayoutModel::note2hue(int note)
{
    float calccol=(float)((note+4)%12)*30;
    return (int)calccol%360;
}
int LayoutModel::getFontsize() const
{
    return fontsize;
}
QString LayoutModel::getFont() const
{
    return font;
}

void LayoutModel::updateLayout()
{
    int seg;
    int calcnote=basenote+(baseoct+(topoct-baseoct)/2)*12;
    for(seg=bscaleStartSeg;seg<bscaleStartSeg+11;seg++) {
        value[seg]=midi2f[++calcnote];
        segH[seg]=note2hue(calcnote);
    }
    valueint[24]=baseoct;
    //value[24]=0.1f*(float)baseoct;
    valueint[25]=topoct;
    //value[25]=0.1f*(float)topoct;

    // row 4: the scale
    seg=scaleStartSeg;
    for(int i=baseoct;i<=topoct;i++) {
        calcnote=basenote+i*12;
        valueint[seg]=calcnote;
        value[seg]=midi2f[calcnote];
        segText[seg]=midi2TextEU[calcnote%12];
        segwidth[seg]=1;
        segtype[seg]=0;
        segH[seg]=note2hue(calcnote);
        for(int j=0;j<11;j++) {
            if(bscale[j]) {
                if(transMode) {
                    seg++;
                    segtype[seg]=1;
                    segText[seg]="";
                    segwidth[seg]=1;
                }
                seg++;
                segtype[seg]=0;
                segwidth[seg]=1;
                int thisnote=calcnote+j+1;
                valueint[seg]=thisnote;
                value[seg]=midi2f[thisnote];
                segText[seg]=midi2TextEU[thisnote%12];
                ctlx[seg]=1;
                ctly[seg]=2;
//                segText[seg].sprintf("%d\n%d",midi2TextEU[thisnote%12],(int)value[seg]);
                segH[seg]=note2hue(thisnote);
//                qDebug() << seg << " segh " << segH[seg];
            }
        }
        seg++;
        if(i<topoct && transMode) {
            segtype[seg]=1;
            segText[seg]="";
            seg++;
        }
    }
    /*
    int calcnote=basenote+(topoct+1)*12;
    valueint[seg]=calcnote;
    value[seg]=midi2f[calcnote];
    segText[seg]=midi2TextEU[calcnote%12];
    segwidth[seg]=1;
    segtype[seg]=0;
    seg++;*/

    nseg[scaleRow]=seg-scaleStartSeg;
    segwidthmax[scaleRow]=seg-scaleStartSeg;
    nsegs=seg;
    calcGeo(width,height);
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
