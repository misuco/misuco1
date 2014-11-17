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
#ifdef RC1_IOS
    font="DIN Alternate";
#else
    font="Roboto";
#endif
    //nrowsmax=32;
    //nsegsmax=32*32;
    nseg = new unsigned int[nrows_max];
    setAll(nrows_max,nseg,0);
    rowheight = new uint[nrows_max];
    setAll(nrows_max,rowheight,1);
    rowheightpx = new uint[nrows_max];
    setAll(nrows_max,rowheightpx,1);
    segwidth = new uint[nsegs_max];
    setAll(nsegs_max,segwidth,1);
    segwidthpx = new uint[nsegs_max];
    setAll(nsegs_max,segwidthpx,1);
    segwidthmax=new uint[nrows_max];
    setAll(nrows_max,segwidthmax,1);
    freq = new float[nsegs_max];
    midinote = new uint[nsegs_max];
    pitch = new int[nsegs_max];
    segText=new QString[nsegs_max];
    segH = new uint[nsegs_max];
    xrel = new float[nsegs_max];
    yrel = new float[nsegs_max];
    layxml.setLayoutModel(this);

    segtype = new uint[nsegs_max];
    setAll(nsegs_max,segtype,0);
    
    ctlx=new uint[nsegs_max];
    setAll(nsegs_max,ctlx,1);
    
    ctly=new uint[nsegs_max];
    setAll(nsegs_max,ctly,2);
    
    chan=new uint[nsegs_max];
    setAll(nsegs_max,chan,0);
    
    pressed=new uint[nsegs_max];
    setAll(nsegs_max,pressed,0);

    xrelq=new uint[nsegs_max];
    setAll(nsegs_max,xrelq,0);

    yrelq=new uint[nsegs_max];
    setAll(nsegs_max,yrelq,0);

    midi2f = new float[256];
    midi2fequal = new float[256];
    midi2fcent = new float[12];
    calcMidi2Fequal();  // initialize equaly tempered midi frequency lut
    initMidi2f(0);      // includes calcMidi2f();
    
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

    midi2TextUrl = new QString[12];
    midi2TextUrl[0]="c";
    midi2TextUrl[1]="cs";
    midi2TextUrl[2]="d";
    midi2TextUrl[3]="ds";
    midi2TextUrl[4]="e";
    midi2TextUrl[5]="f";
    midi2TextUrl[6]="fs";
    midi2TextUrl[7]="g";
    midi2TextUrl[8]="gs";
    midi2TextUrl[9]="a";
    midi2TextUrl[10]="as";
    midi2TextUrl[11]="b";
    
    scaleStartSeg=24;
    scaleRow=4;
    transMode=false;
    currLayout=1;
    editMode=true;

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
    segBorder=0;
    
    qDebug() << "freq_max " << freq_max;
}

void LayoutModel::calcGeo()
{
    calcGeo(widthPx,heightPx);
}

void LayoutModel::calcGeo(uint w, uint h)
{
    //qDebug() << "Cacl geo " << w << " " << h << " " << width << " " << height;
    widthPx=w;
    heightPx=h;
    segBorder=heightPx*10/nrows/100;
    uint i=0;
    uint rowheightsum=0;
    for(uint y=0;y<nrows;y++) {
        // if(aspect_change) {
            rowheightpx[y]=heightPx*rowheight[y]/rowheightmax;
            rowheightsum+=rowheightpx[y];
            // additional pixels may occur due to rounding differences
            // -> add additional pixels to last row
            if(y==nrows-1 && rowheightsum<heightPx) {
                rowheightpx[y]+=rowheightsum-heightPx;
            }
        // }
        uint segwidthsum=0;
        for(uint x=0;x<nseg[y];x++) {
            segwidthpx[i]=widthPx*segwidth[i]/segwidthmax[y];
            segwidthsum+=segwidthpx[i];
            // -> add additional pixels to last segment
            if(x==nseg[y]-1 && segwidthsum<widthPx) {
                segwidthpx[i]+=widthPx-segwidthsum;
            }
            i++;
        }
    }
#ifdef RC1_IOS
    fontsize=heightPx/20;
#else
    fontsize=heightPx/30;
#endif
    fontsizeS=fontsize/2;
}

uint LayoutModel::getHeight() const
{
    return heightPx;
}

uint LayoutModel::getWidth() const
{
    return widthPx;
}

uint LayoutModel::getNrows() const
{
    return nrows;
}

uint LayoutModel::getRowheightpx(uint i) const
{
    if(i<nrows_max) {
        return rowheightpx[i];
    } else {
        return 0;
    }
}

uint LayoutModel::getNseg(uint i) const
{
    if(i<nrows_max) {
        return nseg[i];
    } else {
        return 0;
    }
}

uint LayoutModel::getNsegs() const
{
    return nsegs;
}

void LayoutModel::setAll(uint n, uint *d, uint v)
{
    for(uint i=scaleStartSeg;i<n;i++) {
        d[i]=v;
    }
}

uint LayoutModel::getSegwidth(uint i) const
{
    if(i<nsegs_max) {
        return segwidth[i];
    } else {
        return 0;
    }
}

uint LayoutModel::getCtly(uint i) const
{
    if(i<=nsegs_max) {
        return ctly[i];
    } else {
        return 0;
    }
}

uint LayoutModel::getChan(uint i) const
{
    if(i<nsegs_max) {
        return chan[i];
    } else {
        return 0;
    }
}

uint LayoutModel::getCtlx(uint i) const
{
    if(i<nsegs_max) {
        return ctlx[i];
    } else {
        return 0;
    }
}

float LayoutModel::getFreq(uint i) const
{
    if(i<nsegs_max) {
        return freq[i];
    } else {
        return 0;
    }
}

uint LayoutModel::getMidinote(uint i) const
{
    if(i<nsegs_max) {
        return midinote[i];
    } else {
        return 0;
    }
}

uint LayoutModel::getPitch(uint i) const
{
    if(i<nsegs_max) {
        return pitch[i];
    } else {
        return 0;
    }
}

uint LayoutModel::getSegwidthpx(uint i) const
{
    if(i<nsegs_max) {
        return segwidthpx[i];
    } else {
        return 0;
    }
}

uint LayoutModel::getSegwidthmax(uint i) const
{
    if(i<nrows_max) {
        return segwidthmax[i];
    } else {
        return 0;
    }
}

uint LayoutModel::getPressed(uint i) const
{
    if(i<nsegs_max) {
        return pressed[i];
    } else {
        return 0;
    }
}

uint LayoutModel::getSegtype(uint i) const
{
    if(i<nsegs_max) {
        return segtype[i];
    } else {
        return 0;
    }
}

QString *LayoutModel::getSegText(uint i) const
{
    if(i<nsegs_max) {
        return &(segText[i]);
    } else {
        return NULL;
    }
}

uint LayoutModel::getSegH(uint i) const
{
    if(i<nsegs_max) {
        return segH[i];
    } else {
        return 0;
    }
}

void LayoutModel::incPressed(uint i)
{
    if(i<nsegs_max) {
        pressed[i]++;
    }
}

void LayoutModel::decPressed(uint i)
{
    if(i<nsegs_max) {
        pressed[i]--;
    }
}

double LayoutModel::Log2( double n )
{
    // log(n)/log(2) is log2.
    return log( n ) / log( 2 );
}

float LayoutModel::getXrel(uint i) const
{
    if(i<nsegs_max) {
        return xrel[i];
    } else {
        return 0;
    }
}

void LayoutModel::setXrel(uint i,float value)
{
    if(i<nsegs_max && value>=0) {
        xrel[i] = value;
    }
}

float LayoutModel::getYrel(uint i) const
{
    if(i<nsegs_max) {
        return yrel[i];
    } else {
        return 0;
    }
}

void LayoutModel::setYrel(uint i,float value)
{
    if(i<nsegs_max && value>=0) {
        yrel[i] = value;
    }
}

uint LayoutModel::getXrelq(uint i) const
{
    if(i<nsegs_max) {
        return xrelq[i];
    } else {
        return 0;
    }
}

void LayoutModel::setXrelq(uint i, int value)
{
    if(i<nsegs_max) {
        xrelq[i] = value;
    }
}

uint LayoutModel::getYrelq(uint i) const
{
    if(i<nsegs_max) {
        return yrelq[i];
    } else {
        return 0;
    }
}

void LayoutModel::setYrelq(uint i, int value)
{
    if(i<nsegs_max) {
        yrelq[i] = value;
    }
}

uint LayoutModel::getSoundParam(uint i) const
{
    if(i<progmem.soudnparam_max) {
        return progmem.progmem[actProgmen].soundParam[i];
    } else {
        return 0;
    }
}

uint LayoutModel::getSoundParamMax() const
{
    return progmem.soudnparam_max;
}

void LayoutModel::setSoundParam(uint i, uint value)
{
    if(i<progmem.soudnparam_max) {
        progmem.progmem[actProgmen].soundParam[i] = value;
    }
}

uint LayoutModel::getCurrLayout() const
{
    return currLayout;
}

void LayoutModel::setCurrLayout(uint value)
{
    currLayout = value;
}

uint LayoutModel::getSegBorder() const
{
    return segBorder;
}

void LayoutModel::setFreq(uint i, float f)
{
    if(i<nsegs_max) {
        if(f>=1.0f && f<=freq_max) {
            freq[i]=f;
            midinote[i]=round(Log2(f/(freq_a/64.0))*12.0-3);
            pitch[i]=calcPitch(midinote[i],f);
            segH[i]=note2hue(midinote[i]);
            qDebug() << "LayoutModel::setFreq " << f << " midi " << midinote[i] << " pitch " << pitch[i];
        }
    }
}

float LayoutModel::calcPitch(int midinote, float f) {
    return round(Log2(f/midi2fequal[midinote])*12*8192/2);
}

void LayoutModel::setMidinote(uint i, uint v)
{
    if(i<nsegs_max) {
        if(v<=midinote_max) {
            midinote[i]=v;
            freq[i]=midi2f[v];
            pitch[i]=0;
            segH[i]=note2hue(v);
        }
    }
}

void LayoutModel::setSegtype(uint i, uint v) const
{
    if(i<nsegs_max) {
        segtype[i]=v;
    }
}

void LayoutModel::setRowheight(uint i, uint v) const
{
    if(i<nrows) {
        rowheight[i]=v;
    }
}

void LayoutModel::setRowheightmax(uint v)
{
    rowheightmax=v;
}

void LayoutModel::setSegwidth(uint i, uint v) const
{
    if(i<nsegs_max) {
        segwidth[i]=v;
    }
}

void LayoutModel::setSegwidthmax(uint i, uint v) const
{
    if(i<nrows) {
        segwidthmax[i]=v;
    }
}

void LayoutModel::setChan(uint i, uint v) const
{
    if(i<nsegs_max) {
        chan[i]=v;
    }
}

void LayoutModel::setCtlx(uint i, uint v) const
{
    if(i<nsegs_max) {
        ctlx[i]=v;
    }
}

void LayoutModel::setCtly(uint i, uint v) const
{
    if(i<nsegs_max) {
        ctly[i]=v;
    }
}

void LayoutModel::setPressed(uint i, uint v) const
{
    if(i<nsegs_max) {
        pressed[i]=v;
    }
}

void LayoutModel::setSegtext(uint i, QString t) const
{
    if(i<nsegs_max) {
        segText[i]=t;
    }
}

uint LayoutModel::getBasenote() const
{
    return progmem.progmem[actProgmen].basenote;
}

void LayoutModel::setBasenote(uint v)
{
    if(v<=midinote_max) {
        progmem.progmem[actProgmen].basenote = v;
    }
}

bool LayoutModel::getBscale(uint n)
{
    if(n<progmem.bscale_max) {
        return progmem.progmem[actProgmen].bscale[n];
    } else {
        return false;
    }
}

void LayoutModel::setBscale(uint n, bool v)
{
    if(n<progmem.bscale_max) {
        progmem.progmem[actProgmen].bscale[n]=v;
    }
}

uint LayoutModel::getTopoct() const
{
    return progmem.progmem[actProgmen].topoct;
}

void LayoutModel::setTopoct(uint v)
{
    if(v>10) {
        v=10;
    }
    if(progmem.progmem[actProgmen].baseoct>v) {
        progmem.progmem[actProgmen].topoct=progmem.progmem[actProgmen].baseoct;
        progmem.progmem[actProgmen].baseoct=v;
    } else {
        progmem.progmem[actProgmen].topoct = v;
    }
}

uint LayoutModel::getBaseoct() const
{
    return progmem.progmem[actProgmen].baseoct;
}

void LayoutModel::setBaseoct(uint v)
{
    if(v>10) {
        v=10;
    }
    if(progmem.progmem[actProgmen].topoct<v) {
        progmem.progmem[actProgmen].baseoct=progmem.progmem[actProgmen].topoct;
        progmem.progmem[actProgmen].topoct=v;
    } else {
        progmem.progmem[actProgmen].baseoct=v;
    }
}

void LayoutModel::setTransMode(bool t)
{
    transMode=t;
}

void LayoutModel::setRowheightpx(uint i, uint v)
{
    rowheightpx[i]=v;
}

void LayoutModel::setRowheight(uint i, uint v)
{
    if(i<=nrows_max) {
        rowheight[i]=v;
    }
}

void LayoutModel::setNrows(uint v)
{
    if(v<=nrows_max && v>0) {
        nrows=v;
    }
}

void LayoutModel::setNseg(uint i, uint v)
{
    if(i<=nrows_max) {
        if(nsegs-nseg[i]+v<=nsegs_max) {
            nsegs-=nseg[i];
            nseg[i]=v;
            nsegs+=v;
        }
    }
}

void LayoutModel::setNsegs(uint v)
{
    if(v<=nsegs_max) {
        nsegs=v;
    }
}

uint LayoutModel::getScalerow() const
{
    return scaleRow;
}

void LayoutModel::setScaleStartSeg(uint s)
{
    if(s<nsegs) {
        scaleStartSeg=s;
    }
}

uint LayoutModel::note2hue(uint note)
{
    float calccol=(float)((note+4)%12)*30;
    return (int)calccol%360;
}

float LayoutModel::midi2freq(uint note)
{
    if(note<midinote_max) {
        return midi2f[note];
    } else {
        return 0;
    }
}

float LayoutModel::calcMidi2f(int x)
{
    int oct=(x+3)/12;
    int p=x+3;
    return (freq_a / 64.0f) * (pow(2.0 , (double)((float)oct*1200.0f+(midi2fcent[p%12]+100.0f*(p%12))) / 1200.0));
}

float LayoutModel::calcMidi2Fequal(int x)
{
    int oct=(x+3)/12;
    int p=x+3;
    return (freq_a / 64.0f) * (pow(2.0 , (double)((float)oct*1200.0f+(100.0f*(p%12))) / 1200.0));
}

void LayoutModel::calcMidi2f()
{
    for(uint i=0;i<=midinote_max;i++) {
        midi2f[i]=calcMidi2f(i);
        //qDebug() << "pos " << pos << " i " << i << " value " << value;
    }
}

void LayoutModel::calcMidi2Fequal()
{
    for(int i=0;i<=255;i++) {
        midi2fequal[i]=calcMidi2Fequal(i);
    }
}

void LayoutModel::setMidi2fcent(uint pos, float value)
{
    if(pos<12) {
        midi2fcent[pos]=value;
    }
    calcMidi2f();
}

void LayoutModel::initMidi2f(uint n)
{
    switch(n) {
        case 1:
            // reinen Stimmung von C-Dur und C-moll
            // http://de.wikipedia.org/wiki/Gleichstufige_Stimmung
            midi2fcent[0]=16;
            midi2fcent[1]=28;
            midi2fcent[2]=20;
            midi2fcent[3]=32;
            midi2fcent[4]=2;
            midi2fcent[5]=14;
            midi2fcent[6]=6;
            midi2fcent[7]=18;
            midi2fcent[8]=30;
            midi2fcent[9]=0;
            midi2fcent[10]=34;
            midi2fcent[11]=4;
            break;

        case 2:
            // Bach (Billeter, Wohltemperiert)
            // http://www.instrument-tuner.com/temperaments_de.html
            midi2fcent[0]=4.888;
            midi2fcent[1]=-2.932;
            midi2fcent[2]=4.888;
            midi2fcent[3]=0.978;
            midi2fcent[4]=-4.887;
            midi2fcent[5]=4.888;
            midi2fcent[6]=-4.887;
            midi2fcent[7]=4.888;
            midi2fcent[8]=-0.977;
            midi2fcent[9]=0;
            midi2fcent[10]=2.933;
            midi2fcent[11]=-4.887;
            break;
            
        case 3:
            // Natürlich Harmonisch
            // http://www.instrument-tuner.com/temperaments_de.html
            midi2fcent[0]=15.641;
            midi2fcent[1]=27.372;
            midi2fcent[2]=19.551;
            midi2fcent[3]=31.282;
            midi2fcent[4]=1.956;
            midi2fcent[5]=13.686;
            midi2fcent[6]=5.864;
            midi2fcent[7]=17.596;
            midi2fcent[8]=29.327;
            midi2fcent[9]=0;
            midi2fcent[10]=33.237;
            midi2fcent[11]=3.910;
            break;

        case 4:
            // Pythagoreisch
            // http://www.instrument-tuner.com/temperaments_de.html
            midi2fcent[0]=-5.865;
            midi2fcent[1]=7.820;
            midi2fcent[2]=-1.955;
            midi2fcent[3]=-11.730;
            midi2fcent[4]=1.955;
            midi2fcent[5]=-7.820;
            midi2fcent[6]=5.865;
            midi2fcent[7]=-3.910;
            midi2fcent[8]=9.775;
            midi2fcent[9]=0.000;
            midi2fcent[10]=-9.775;
            midi2fcent[11]=3.910;
            break;

    default:
        for(int i=0;i<12;i++) {
            midi2fcent[i]=0;
        }
    }
    calcMidi2f();
}

void LayoutModel::toggleEdit()
{
    if(!editMode) {
        editMode=true;
        resetLayout();
    } else {
        rowheight[scaleRow-1]=10;
        rowheight[scaleRow]=rowheightmax-10;
        editMode=false;
        for(uint i=0;i<nrows;i++) {
            if(i<scaleRow-1) {
                rowheight[i]=0;
            }
        }
    }
    calcGeo();
    updateLayout();
}


void LayoutModel::resetLayout(uint i) {
    QString filename;
    filename.sprintf(":/conf/l%d.xml",i);
    currLayout=i;
    resetLayout(filename);
}

void LayoutModel::resetLayout(QString filename) {
    layxml.readXml(filename);
}

void LayoutModel::resetLayout() {
    resetLayout(currLayout);
}


uint LayoutModel::getFontsize() const
{
    return fontsize;
}

uint LayoutModel::getFontsizeS() const
{
    return fontsizeS;
}
QString LayoutModel::getFont() const
{
    return font;
}

QString LayoutModel::getMidi2Text(uint midi) const
{
    return midi2TextEU[midi%12];
}

QString LayoutModel::getMidi2TextUrl(uint midi) const
{
    return midi2TextUrl[midi%12];
}

void LayoutModel::updateLayout()
{
    uint seg=0;

    //qDebug() << "scaleStartSeg " << scaleStartSeg << " nsegs " << nsegs;
    // switch states for edit elements
    for(seg=0;seg<scaleStartSeg;seg++) {
        if(segtype[seg]==0 ) {
            int note=0;
            if(ctly[seg]==-1) {
                note=ctlx[seg];
                if(progmem.progmem[actProgmen].basenote==ctlx[seg]) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            } else if(ctly[seg]==-2) {
                note=(progmem.progmem[actProgmen].basenote+ctlx[seg]+1)%12;
                if(progmem.progmem[actProgmen].bscale[ctlx[seg]]) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            } else if(ctly[seg]==-3) {
                note=ctlx[seg]%12;
                yrel[seg]=midi2fcent[(note+3)%12]/200+0.5;
            }
            int oct=progmem.progmem[actProgmen].baseoct+progmem.progmem[actProgmen].topoct;
            oct/=2;
            oct*=12;
            midinote[seg]=note+oct;
            freq[seg]=midi2f[note+oct];
            pitch[seg]=midi2fcent[(note+3)%12]*4096.0f/100.0f;
            segH[seg]=note2hue(note);
            segText[seg]=midi2TextEU[note%12];
        } else if(segtype[seg]==2 ) {
            if(ctly[seg]==-2) {
                if(actProgmen==ctlx[seg]) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            } else if(ctly[seg]==-3) {
                    if(editMode) {
                        pressed[seg]=1;
                    } else {
                        pressed[seg]=0;
                    }
            } else if(ctly[seg]==-4) {
                if(ctlx[seg]==currLayout) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            }
        } else if(segtype[seg]==3 ) {
            if(ctly[seg]==-2) {
                if(transMode) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            }
        } else if(segtype[seg]==4 ) {
            if(ctly[seg]==-2) {
                xrelq[seg]=actProgmen;
            } else if(ctly[seg]==-4) {
                xrelq[seg]=progmem.progmem[actProgmen].topoct;
            } else if(ctly[seg]==-5) {
                xrelq[seg]=progmem.progmem[actProgmen].baseoct;
//            } else if(ctly[seg]>0 && ctly[seg]<NSOUNDPARAM) {
//                xrelq[seg]=progmem.progmem[actProgmen].soundParam[ctly[seg]];
            }
        } else if(segtype[seg]==5 ) {
            if(ctly[seg]>=102) {
                yrelq[seg]=progmem.progmem[actProgmen].soundParam[ctly[seg]-102];
            }
        }
    }
    seg=generateScale(seg);
    nseg[scaleRow]=seg-scaleStartSeg;
    segwidthmax[scaleRow]=seg-scaleStartSeg;
    nsegs=seg;
    calcGeo(widthPx,heightPx);
}

uint LayoutModel::generateScale(uint seg) {
    // row 4: the scale
    //seg=scaleStartSeg;
    if(seg>=nsegs_max) return seg;
    uint startseg=seg; // save startseg to count added segs

    for(uint i=progmem.progmem[actProgmen].baseoct;i<=progmem.progmem[actProgmen].topoct;i++) {
        int calcnote=progmem.progmem[actProgmen].basenote+i*12;
        midinote[seg]=calcnote;
        freq[seg]=midi2f[calcnote];
        pitch[seg]=midi2fcent[(calcnote+3)%12]*4096.0f/100.0f;
        QString octnum;
        octnum.sprintf(" %d",i+1);
        segText[seg]=midi2TextEU[calcnote%12];
        segText[seg].append(octnum);
        ctlx[seg]=1;
        ctly[seg]=2;
        segwidth[seg]=1;
        segtype[seg]=0;
        segH[seg]=note2hue(calcnote);
        pressed[seg]=0;
        for(int j=0;j<11;j++) {
            if(progmem.progmem[actProgmen].bscale[j]) {
                if(transMode) {
                    seg++;
                    if(seg>=nsegs_max) return seg;
                    segtype[seg]=1;
                    segText[seg]="";
                    segwidth[seg]=1;
                    ctlx[seg]=1;
                    ctly[seg]=2;
                    pressed[seg]=0;
                }
                seg++;
                if(seg>=nsegs_max) return seg;
                segtype[seg]=0;
                segwidth[seg]=1;
                int thisnote=calcnote+j+1;
                midinote[seg]=thisnote;
                freq[seg]=midi2f[thisnote];
                pitch[seg]=midi2fcent[(thisnote+3)%12]*4096.0f/100.0f;
                //qDebug() << "pitch " << pitch[seg] << " " ;
                segText[seg]=midi2TextEU[thisnote%12];
                segText[seg].append(octnum);
                ctlx[seg]=1;
                ctly[seg]=2;
                segH[seg]=note2hue(thisnote);
                pressed[seg]=0;
            }
        }
        seg++;
        if(seg>=nsegs_max) return seg;
        if(i<progmem.progmem[actProgmen].topoct && transMode) {
            segtype[seg]=1;
            segText[seg]="";
            segwidth[seg]=1;
            pressed[seg]=0;
            ctlx[seg]=1;
            ctly[seg]=2;
            seg++;
            if(seg>=nsegs_max) return seg;
        }
    }

    // it sounds nice if an octave finishes
    // on top with the first note
    //

    // but only, if there is more than 1 seg
    if(seg>startseg+1) {

        // is there enough space in transMode?
        if(transMode) {
            if(seg+1<nsegs_max) {
                segtype[seg]=1;
                segText[seg]="";
                segwidth[seg]=1;
                pressed[seg]=0;
                ctlx[seg]=1;
                ctly[seg]=2;
                seg++;
            }
        }

        // draw top segment only
        // in case that we're  not at the really upper end
        // border (nsegsmax) of the seg storage
        if(!(transMode && seg+1>nsegs_max)) {
            int calcnote=progmem.progmem[actProgmen].basenote+(progmem.progmem[actProgmen].topoct+1)*12;
            midinote[seg]=calcnote;
            freq[seg]=midi2f[calcnote];
            segText[seg]=midi2TextEU[calcnote%12];
            segwidth[seg]=1;
            segtype[seg]=0;
            ctlx[seg]=1;
            ctly[seg]=2;
            segH[seg]=note2hue(calcnote);
            pressed[seg]=0;
            seg++;
        }
    }
    return seg;
}


/*
void LayoutModel::setSegH(uint i, uint v)
{
    if(i<nsegs_max) {
        segH[i]=v;
    }
}
 */

void LayoutModel::setAllCtlx(uint v)
{
    setAll(nsegs_max,ctlx,v);
}

void LayoutModel::setAllCtly(uint v)
{
    setAll(nsegs_max,ctly,v);
}

void LayoutModel::setAllChan(uint v)
{
    setAll(nsegs_max,chan,v);
}

void LayoutModel::setActProgmem(uint n)
{
    // restore new setup
    if(n<progmem.progmem_max) {
        actProgmen=n;
        updateLayout();
    }
}

bool LayoutModel::getEditMode() const
{
    return editMode;
}

void LayoutModel::readProgmemXml(QString filename)
{
    progmem.readProgmemXml(filename);
}

void LayoutModel::writeProgmemXml(QString filename)
{
    progmem.writeProgmemXml(filename);
}

