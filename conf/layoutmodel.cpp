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
#include <QFile>
#include "layoutmodel.h"
#include "layoutxml.h"


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
        progmem[actProgmen].bscale[i]=false;
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

    midi2f = new float[256];
    midi2fcent = new float[12];
    initMidi2f(0);

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
//    calcGeo(200,200);
//    updateLayout();
}

void LayoutModel::calcGeo()
{
    calcGeo(widthPx,heightPx);
}

void LayoutModel::calcGeo(int w, int h)
{
    //qDebug() << "Cacl geo " << w << " " << h << " " << width << " " << height;
    widthPx=w;
    heightPx=h;
    segBorder=heightPx*10/nrows/100;
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
    fontsize=heightPx/25;
    fontsizeS=fontsize/2;
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
    if(i<NSOUNDPARAM) {
        return progmem[actProgmen].soundParam[i];
    } else {
        return 0;
    }
}

void LayoutModel::setSoundParam(int i, int value)
{
    if(i<NSOUNDPARAM) {
        progmem[actProgmen].soundParam[i] = value;
    }
}
int LayoutModel::getCurrLayout() const
{
    return currLayout;
}

void LayoutModel::setCurrLayout(int value)
{
    currLayout = value;
}
int LayoutModel::getSegBorder() const
{
    return segBorder;
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
    return progmem[actProgmen].basenote;
}

void LayoutModel::setBasenote(int v)
{
    progmem[actProgmen].basenote = v;
}

bool LayoutModel::getBscale(int n)
{
    return progmem[actProgmen].bscale[n];
}

void LayoutModel::setBscale(int n, bool v)
{
    progmem[actProgmen].bscale[n]=v;
}

int LayoutModel::getTopoct() const
{
    return progmem[actProgmen].topoct;
}

void LayoutModel::setTopoct(int v)
{
    if(progmem[actProgmen].baseoct>v) {
        progmem[actProgmen].topoct=progmem[actProgmen].baseoct;
        progmem[actProgmen].baseoct=v;
    } else {
        progmem[actProgmen].topoct = v;
    }
}

int LayoutModel::getBaseoct() const
{
    return progmem[actProgmen].baseoct;
}

void LayoutModel::setBaseoct(int v)
{
    if(progmem[actProgmen].topoct<v) {
        progmem[actProgmen].baseoct=progmem[actProgmen].topoct;
        progmem[actProgmen].topoct=v;
    } else {
        progmem[actProgmen].baseoct=v;
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


float LayoutModel::calcMidi2f(int x)
{
    int oct=(x+3)/12;
    int p=x+3;
    return (freq_a / 64.0f) * (pow(2.0 , (double)((float)oct*1200.0f+(midi2fcent[p%12]+100.0f*(p%12))) / 1200.0));
}

void LayoutModel::calcMidi2f()
{
    /*

    // set only the changed strings to save time
    int i=pos-3;
    if(i<0) {
        i+=12;
    }
    for(;i<=255;i+=12) {
        midi2f[i]=calcMidi2f(i);
        //qDebug() << "pos " << pos << " i " << i << " value " << value;
    }
    */
    for(int i=0;i<=255;i++) {
        midi2f[i]=calcMidi2f(i);
        //qDebug() << "pos " << pos << " i " << i << " value " << value;
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
        for(int i=0;i<nrows;i++) {
            if(i<scaleRow-1) {
                rowheight[i]=0;
            }
        }
    }
    calcGeo();
    updateLayout();
}


void LayoutModel::resetLayout(int i) {
    QString filename;
    filename.sprintf(":/conf/l%d.xml",i);
    currLayout=i;
    resetLayout(filename);
}

void LayoutModel::resetLayout(QString filename) {
    layoutxml lxml;
    lxml.setLayoutModel(this);
    lxml.readXml(filename);
}

void LayoutModel::resetLayout() {
    resetLayout(currLayout);
}


int LayoutModel::getFontsize() const
{
    return fontsize;
}

int LayoutModel::getFontsizeS() const
{
    return fontsizeS;
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

    //qDebug() << "scaleStartSeg " << scaleStartSeg << " nsegs " << nsegs;
    // switch states for edit elements
    for(seg=0;seg<scaleStartSeg;seg++) {
        if(segtype[seg]==0 ) {
            int note=0;
            if(ctly[seg]==-1) {
                note=ctlx[seg];
                if(progmem[actProgmen].basenote==ctlx[seg]) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            } else if(ctly[seg]==-2) {
                note=(progmem[actProgmen].basenote+ctlx[seg]+1)%12;
                if(progmem[actProgmen].bscale[ctlx[seg]]) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            } else if(ctly[seg]==-3) {
                note=ctlx[seg]%12;
                yrel[seg]=midi2fcent[(note+3)%12]/200+0.5;
            }
            int oct=progmem[actProgmen].baseoct+progmem[actProgmen].topoct;
            oct/=2;
            oct*=12;
            midinote[seg]=note+oct;
            freq[seg]=midi2f[note+oct];
            pitch[seg]=0;
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
                xrelq[seg]=progmem[actProgmen].topoct;
            } else if(ctly[seg]==-5) {
                xrelq[seg]=progmem[actProgmen].baseoct;
//            } else if(ctly[seg]>0 && ctly[seg]<NSOUNDPARAM) {
//                xrelq[seg]=progmem[actProgmen].soundParam[ctly[seg]];
            }
        } else if(segtype[seg]==5 ) {
            if(ctly[seg]>117) {
                yrelq[seg]=progmem[actProgmen].soundParam[ctly[seg]-118];
            }
        }
    }

    // row 4: the scale
    //seg=scaleStartSeg;
    for(int i=progmem[actProgmen].baseoct;i<=progmem[actProgmen].topoct;i++) {
        int calcnote=progmem[actProgmen].basenote+i*12;
        midinote[seg]=calcnote;
        freq[seg]=midi2f[calcnote];
        pitch[seg]=0;
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
            if(progmem[actProgmen].bscale[j]) {
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
        if(i<progmem[actProgmen].topoct && transMode) {
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


void LayoutModel::readProgmemXml(QString filename)
{
    QXmlStreamReader xmlr;
    QFile file(filename);
    // default initial prog memory
    qDebug() << "reading progmem " << filename;
    if(!file.exists()) {
        qDebug() << "init progmem file not exist ";
        for(int i=0;i<NPROGMEM;i++) {
            progmem[i].basenote=i%5;
            progmem[i].baseoct=3;
            progmem[i].topoct=5;
            for(int j=0;j<11;j++) {
                progmem[i].bscale[j]=false;
            }
            progmem[i].bscale[i]=true;
            progmem[i].bscale[i%4]=true;

            progmem[i].soundParam[0]=2;     // waveform
            progmem[i].soundParam[1]=10;    // attack
            progmem[i].soundParam[2]=10;    // decay
            progmem[i].soundParam[3]=127;   // sustain
            progmem[i].soundParam[4]=64;    // release
            progmem[i].soundParam[5]=0;     // cutoff
            progmem[i].soundParam[6]=12*i;  // resonance
            progmem[i].soundParam[7]=0;     // mod cutoff
            progmem[i].soundParam[8]=0;     // mod resonance
            progmem[i].soundParam[9]=100;   // volume
        }
        writeProgmemXml(filename);
    }
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
        qDebug() << "cannot read file " << filename;
    }
    xmlr.setDevice(&file);
    if (xmlr.readNextStartElement()) {
        if (xmlr.name() == "misucoprogmem" && xmlr.attributes().value("version") == "1.0") {

            int row=0;

            while (xmlr.readNextStartElement() && row<NPROGMEM) {
                if (xmlr.name() == "prog") {
                    progmem[row].basenote=xmlr.attributes().value("basenote").toString().toInt();
                    progmem[row].baseoct=xmlr.attributes().value("baseoct").toString().toInt();
                    progmem[row].topoct=xmlr.attributes().value("topoct").toString().toInt();
                    progmem[row].bscale[0]=(bool)xmlr.attributes().value("bscale0").toString().toInt();
                    progmem[row].bscale[1]=(bool)xmlr.attributes().value("bscale1").toString().toInt();
                    progmem[row].bscale[2]=(bool)xmlr.attributes().value("bscale2").toString().toInt();
                    progmem[row].bscale[3]=(bool)xmlr.attributes().value("bscale3").toString().toInt();
                    progmem[row].bscale[4]=(bool)xmlr.attributes().value("bscale4").toString().toInt();
                    progmem[row].bscale[5]=(bool)xmlr.attributes().value("bscale5").toString().toInt();
                    progmem[row].bscale[6]=(bool)xmlr.attributes().value("bscale6").toString().toInt();
                    progmem[row].bscale[7]=(bool)xmlr.attributes().value("bscale7").toString().toInt();
                    progmem[row].bscale[8]=(bool)xmlr.attributes().value("bscale8").toString().toInt();
                    progmem[row].bscale[9]=(bool)xmlr.attributes().value("bscale9").toString().toInt();
                    progmem[row].bscale[10]=(bool)xmlr.attributes().value("bscale10").toString().toInt();
                    for(int i=0;i<NSOUNDPARAM;i++) {
                        QString attrName;
                        attrName.sprintf("soundparam%d",i);
                        progmem[row].soundParam[i]=xmlr.attributes().value(attrName).toInt();
                    }
                    xmlr.skipCurrentElement();
                    row++;
                } else {
                    xmlr.skipCurrentElement();
                }
            }
        } else {
            xmlr.raiseError(QObject::tr("The file is not a MISUCO version 1.0 file."));
        }
    }
    file.close();
}

void LayoutModel::writeProgmemXml(QString filename)
{
    QXmlStreamWriter xml;
    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
        qDebug() << "cannot write file " << filename;
    }
    qDebug() << "writing progmem file " << filename;

    xml.setDevice(&file);
    QString att;
    QString attname;

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE misuco>");
    xml.writeStartElement("misucoprogmem");
    xml.writeAttribute("version", "1.0");

    for (int row = 0; row < NPROGMEM; row++) {
        xml.writeStartElement("prog");

        att.sprintf("%d",progmem[row].basenote);
        xml.writeAttribute("basenote",att);

        att.sprintf("%d",progmem[row].baseoct);
        xml.writeAttribute("baseoct",att);

        att.sprintf("%d",progmem[row].topoct);
        xml.writeAttribute("topoct",att);

        for(int i=0;i<NSOUNDPARAM;i++) {
            att.sprintf("%d",progmem[row].soundParam[i]);
            QString attrName;
            attrName.sprintf("soundparam%d",i);
            xml.writeAttribute(attrName,att);
        }

        for(int j=0;j<11;j++) {
            att.sprintf("%d",(int)progmem[row].bscale[j]);
            attname.sprintf("bscale%d",j);
            xml.writeAttribute(attname,att);
        }

        xml.writeEndElement();
    }
    xml.writeEndDocument();
    file.close();
}


void LayoutModel::setActProgmem(int n)
{
    // restore new setup
    actProgmen=n;
    updateLayout();
}

bool LayoutModel::getEditMode() const
{
    return editMode;
}

