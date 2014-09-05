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
    widthPx=1;
    heightPx=1;
    fontsize=1;
    font="Arial";
    nrowsmax=32;
    nsegsmax=32*32;
    nseg = new int[nrowsmax];
    rowheight = new int[nrowsmax];
    rowheightpx = new int[nrowsmax];
    segwidth = new int[nsegsmax];
    setAll(nsegsmax,segwidth,1);
    segwidthpx = new int[nsegsmax];
    segwidthmax=new int[nrowsmax];
    freq = new float[nsegsmax];
    midinote = new int[nsegsmax];
    pitch = new int[nsegsmax];
    segText=new QString[nsegsmax];
    segH = new int[nsegsmax];
    xrel = new float[nsegsmax];
    yrel = new float[nsegsmax];
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

    xrelq=new int[nsegsmax];
    setAll(nsegsmax,xrelq,0);

    yrelq=new int[nsegsmax];
    setAll(nsegsmax,yrelq,0);

    nSoundParam=128;
    soundParam=new int[nSoundParam];
    setAll(nSoundParam,soundParam,0);

    midi2f = new float[256];
    midi2fcent = new float[12];
    for(int i=0;i<12;i++) {
        midi2fcent[i]=i*100;
    }
    freq_a = 440; // a is 440 hz...

    for (int x = 0; x < 256; ++x)
    {
        midi2f[x] = calcMidi2f(x);
        //qDebug() << "note " << x << " f " << midi2f[x] << " oct " << oct;
    }
    
    midi2TextEU = new QString[12];
    midi2TextEU[0]="C";
    midi2TextEU[1]="Db";
    midi2TextEU[2]="D";
    midi2TextEU[3]="Eb";
    midi2TextEU[4]="E";
    midi2TextEU[5]="F";
    midi2TextEU[6]="Gb";
    midi2TextEU[7]="G";
    midi2TextEU[8]="Ab";
    midi2TextEU[9]="A";
    midi2TextEU[10]="Bb";
    midi2TextEU[11]="B";

    midi2TextUrl = new QString[12];
    midi2TextUrl[0]="c";
    midi2TextUrl[1]="db";
    midi2TextUrl[2]="d";
    midi2TextUrl[3]="eb";
    midi2TextUrl[4]="e";
    midi2TextUrl[5]="f";
    midi2TextUrl[6]="gb";
    midi2TextUrl[7]="g";
    midi2TextUrl[8]="ab";
    midi2TextUrl[9]="a";
    midi2TextUrl[10]="bb";
    midi2TextUrl[11]="b";
    
    basenote=0;
    topoct=6;
    baseoct=3;
    //bscaleStartSeg=0;
    //bscaleRow=0;
    scaleStartSeg=24;
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

void LayoutModel::calcGeo()
{
    calcGeo(widthPx,heightPx);
}

void LayoutModel::calcGeo(int w, int h)
{
    /*
    bool aspect_change=false;
    if(height!=h) {
        aspect_change=true;
    }
    */
    //qDebug() << "Cacl geo " << w << " " << h << " " << width << " " << height;
    widthPx=w;
    heightPx=h;
    int i=0;
    int rowheightsum=0;
    for(int y=0;y<nrows;y++) {
        // if(aspect_change) {
            rowheightpx[y]=heightPx*rowheight[y]/rowheightmax;
            rowheightsum+=rowheightpx[y];
            // additional pixels may occur due to rounding differences
            // -> add additional pixels to last row
            if(y==nrows-1 && rowheightsum<heightPx) {
                rowheightpx[y]+=rowheightsum-heightPx;
            }
        // }
        int segwidthsum=0;
        for(int x=0;x<nseg[y];x++) {
            segwidthpx[i]=widthPx*segwidth[i]/segwidthmax[y];
            segwidthsum+=segwidthpx[i];
            // -> add additional pixels to last segment
            if(x==nseg[y]-1 && segwidthsum<widthPx) {
                segwidthpx[i]+=widthPx-segwidthsum;
            }
            i++;
        }
    }
    fontsize=heightPx/nrows/5;
}

int LayoutModel::getHeight() const
{
    return heightPx;
}

int LayoutModel::getWidth() const
{
    return widthPx;
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
    for(int i=scaleStartSeg;i<n;i++) {
        d[i]=v;
    }
}

float LayoutModel::calcMidi2f(int x)
{
    int oct=(x+3)/12;
    int p=x+3;
    return (freq_a / 64.0f) * (pow(2.0 , (double)((float)oct*1200.0f+(midi2fcent[p%12])) / 1200.0));
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

float LayoutModel::getFreq(int i) const
{
    return freq[i];
}

int LayoutModel::getMidinote(int i) const
{
    return midinote[i];
}

int LayoutModel::getPitch(int i) const
{
    return pitch[i];
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

double LayoutModel::Log2( double n )
{
    // log(n)/log(2) is log2.
    return log( n ) / log( 2 );
}

float LayoutModel::getXrel(int i) const
{
    return xrel[i];
}

void LayoutModel::setXrel(int i,float value)
{
    xrel[i] = value;
}

float LayoutModel::getYrel(int i) const
{
    return yrel[i];
}

void LayoutModel::setYrel(int i,float value)
{
    yrel[i] = value;
}

int LayoutModel::getXrelq(int i) const
{
    return xrelq[i];
}

void LayoutModel::setXrelq(int i, int value)
{
    xrelq[i] = value;
}

int LayoutModel::getYrelq(int i) const
{
    return yrelq[i];
}

void LayoutModel::setYrelq(int i, int value)
{
    yrelq[i] = value;
}
int LayoutModel::getSoundParam(int i) const
{
    return soundParam[i];
}

void LayoutModel::setSoundParam(int i, int value)
{
    soundParam[i] = value;
}


void LayoutModel::setFreq(int i, float v)
{
    if(i<nsegsmax) {
        freq[i]=v;
        pitch[i]=round(Log2(v/midi2f[midinote[i]])*12*8192/2);
    }
}

void LayoutModel::setMidinote(int i, int v)
{
    if(i<nsegsmax) {
        midinote[i]=v;
        freq[i]=midi2f[v];
        pitch[i]=0;
        segH[i]=note2hue(v);
    }
}

void LayoutModel::setSegtype(int i, int v) const
{
    if(i<nsegsmax) {
        segtype[i]=v;
    }
}

void LayoutModel::setRowheight(int i, int v) const
{
    if(i<nrows) {
        rowheight[i]=v;
    }
}

void LayoutModel::setRowheightmax(int v)
{
    rowheightmax=v;
}

void LayoutModel::setSegwidth(int i, int v) const
{
    if(i<nsegsmax) {
        segwidth[i]=v;
    }
}

void LayoutModel::setSegwidthmax(int i, int v) const
{
    if(i<nrows) {
        segwidthmax[i]=v;
    }
}

void LayoutModel::setChan(int i, int v) const
{
    if(i<nsegsmax) {
        chan[i]=v;
    }
}

void LayoutModel::setCtlx(int i, int v) const
{
    if(i<nsegsmax) {
        ctlx[i]=v;
    }
}

void LayoutModel::setCtly(int i, int v) const
{
    if(i<nsegsmax) {
        ctly[i]=v;
    }
}

void LayoutModel::setPressed(int i, int v) const
{
    pressed[i]=v;
}

void LayoutModel::setSegtext(int i, QString t) const
{
    if(i<nsegsmax) {
        segText[i]=t;
    }
}

int LayoutModel::getBasenote() const
{
    return basenote;
}

void LayoutModel::setBasenote(int v)
{
    basenote = v;
}

bool LayoutModel::getBscale(int n)
{
    return bscale[n];
}

void LayoutModel::setBscale(int n, bool v)
{
    bscale[n]=v;
}

int LayoutModel::getTopoct() const
{
    return topoct;
}

void LayoutModel::setTopoct(int v)
{
    if(baseoct>v) {
        topoct=baseoct;
        baseoct=v;
    } else {
        topoct = v;
    }
}

int LayoutModel::getBaseoct() const
{
    return baseoct;
}

void LayoutModel::setBaseoct(int v)
{
    if(topoct<v) {
        baseoct=topoct;
        topoct=v;
    } else {
        baseoct=v;
    }
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

void LayoutModel::setNrows(int v)
{
    if(v<nrowsmax) {
        nrows=v;
    }
}

void LayoutModel::setNseg(int i, int v)
{
    if(i<nrowsmax && v<nrowsmax) {
        nsegs-=nseg[i];
        nseg[i]=v;
        nsegs+=v;
    }
}

void LayoutModel::setNsegs(int v)
{
    if(v<nsegsmax) {
        nsegs=v;
    }
}

int LayoutModel::getScalerow() const
{
    return scaleRow;
}

void LayoutModel::setScaleStartSeg(int s)
{
    if(s<nsegs) {
        scaleStartSeg=s;
    }
}

int LayoutModel::note2hue(int note)
{
    float calccol=(float)((note+4)%12)*30;
    return (int)calccol%360;
}

int LayoutModel::midi2freq(uint note)
{
    if(note<256) {
        return midi2f[note];
    } else {
        return 0;
    }
}

void LayoutModel::setMidi2fcent(uint pos, float value)
{
    if(pos<12) {
        midi2fcent[pos]=value;

        /*
        for (int x = 0; x < 255; ++x)
        {
            midi2f[x] = calcMidi2f(x);
        }
        */

        // set only the changed strings to save time
        int i=pos-3;
        if(i<0) {
            i+=12;
        }
        for(;i<=255;i+=12) {
            midi2f[i]=calcMidi2f(i);
            //qDebug() << "pos " << pos << " i " << i << " value " << value;
        }
    }
}

int LayoutModel::getFontsize() const
{
    return fontsize;
}
QString LayoutModel::getFont() const
{
    return font;
}

QString LayoutModel::getMidi2Text(int midi) const
{
    return midi2TextEU[midi];
}

QString LayoutModel::getMidi2TextUrl(int midi) const
{
    return midi2TextUrl[midi];
}

void LayoutModel::updateLayout()
{
    int seg=0;

    // qDebug() << "scaleStartSeg " << scaleStartSeg << " nsegs " << nsegs;
    // switch states for edit elements
    for(seg=0;seg<scaleStartSeg;seg++) {
        if((segtype[seg]==3 && ctly[seg]==-1) || (segtype[seg]==0 && ctly[seg]==-2)  || (segtype[seg]==0 && ctly[seg]==-1) ) {
            int note;
            if(ctly[seg]==-1) {
                note=ctlx[seg];
                if(basenote==ctlx[seg]) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            } else {
                note=(basenote+ctlx[seg]+1)%12;
                if(bscale[ctlx[seg]]) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            }
            int oct=baseoct*12;
            midinote[seg]=note+oct;
            freq[seg]=midi2f[note+oct];
            pitch[seg]=0;
            segH[seg]=note2hue(note);
            segText[seg]=midi2TextEU[note%12];
        } else if(segtype[seg]==4 ) {
            if(ctly[seg]==-4) {
                xrelq[seg]=topoct;
            } else if(ctly[seg]==-5) {
                xrelq[seg]=baseoct;
            } else if(ctly[seg]>0 && ctly[seg]<nSoundParam) {
                xrelq[seg]=soundParam[ctly[seg]];
            }
        }
    }

    // row 4: the scale
    //seg=scaleStartSeg;
    for(int i=baseoct;i<=topoct;i++) {
        int calcnote=basenote+i*12;
        midinote[seg]=calcnote;
        freq[seg]=midi2f[calcnote];
        pitch[seg]=0;
        QString octnum;
        octnum.sprintf("%d",i+1);
        segText[seg]=midi2TextEU[calcnote%12];
        segText[seg].append(octnum);
        ctlx[seg]=1;
        ctly[seg]=2;
        segwidth[seg]=1;
        segtype[seg]=0;
        segH[seg]=note2hue(calcnote);
        pressed[seg]=0;
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
                midinote[seg]=thisnote;
                freq[seg]=midi2f[thisnote];
                segText[seg]=midi2TextEU[thisnote%12];
                segText[seg].append(octnum);
                ctlx[seg]=1;
                ctly[seg]=2;
                segH[seg]=note2hue(thisnote);
                pressed[seg]=0;
            }
        }
        seg++;
        if(i<topoct && transMode) {
            segtype[seg]=1;
            segText[seg]="";
            pressed[seg]=0;
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

    if(scaleStartSeg>0) {
        nseg[scaleRow]=seg-scaleStartSeg;
        segwidthmax[scaleRow]=seg-scaleStartSeg;
    }
    nsegs=seg;
    calcGeo(widthPx,heightPx);
}

void LayoutModel::setSegH(int i, int v)
{
    segH[i]=v;
}

void LayoutModel::setAllCtlx(int v)
{
    setAll(nsegsmax,ctlx,v);
}

void LayoutModel::setAllCtly(int v)
{
    setAll(nsegsmax,ctly,v);
}

void LayoutModel::setAllChan(int v)
{
    setAll(nsegsmax,chan,v);
}
