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
#include "platform.h"

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

#ifdef RC1_PRO
    confpath=":/conf/lp%d.xml";
#else
    confpath=":/conf/l%d.xml";
#endif

    //nrowsmax=32;
    //nsegsmax=32*32;
    nseg = new int[nrows_max];
    setAll(nrows_max,nseg,0);
    rowheight = new int[nrows_max];
    setAll(nrows_max,rowheight,1);
    rowheightpx = new int[nrows_max];
    setAll(nrows_max,rowheightpx,1);
    segwidth = new int[nsegs_max];
    setAll(nsegs_max,segwidth,1);
    segwidthpx = new int[nsegs_max];
    setAll(nsegs_max,segwidthpx,1);
    segwidthmax=new int[nrows_max];
    setAll(nrows_max,segwidthmax,1);
    freq = new float[nsegs_max];
    midinote = new int[nsegs_max];
    pitch = new int[nsegs_max];
    segText=new QString[nsegs_max];
    segH = new int[nsegs_max];
    xrel = new float[nsegs_max];
    yrel = new float[nsegs_max];
    layxml.setLayoutModel(this);

    segtype = new int[nsegs_max];
    setAll(nsegs_max,segtype,0);
    
    ctlx=new int[nsegs_max];
    setAll(nsegs_max,ctlx,1);
    
    ctly=new int[nsegs_max];
    setAll(nsegs_max,ctly,2);
    
    chan=new int[nsegs_max];
    setAll(nsegs_max,chan,0);
    
    pressed=new int[nsegs_max];
    setAll(nsegs_max,pressed,0);

    xrelq=new int[nsegs_max];
    setAll(nsegs_max,xrelq,0);

    yrelq=new int[nsegs_max];
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
    
    //qDebug() << "freq_max " << freq_max;
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
            //qDebug() << "rowheightsum " << rowheightsum << "+=" << rowheightpx[y];
            // additional pixels may occur due to rounding differences
            // -> add additional pixels to last row
            if(y==nrows-1 && rowheightsum<heightPx) {
                rowheightpx[y]+=heightPx-rowheightsum;
                //qDebug() << "calcGeo: add to px last row: " << rowheightsum-heightPx << " heightPx: " << heightPx;
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
#ifdef RC1_IOS
    fontsize=heightPx/20;
#else
    fontsize=heightPx/30;
#endif
    fontsizeS=fontsize/3*2;

    waveDisplayY=0;
    for(int i=0;i<getScalerow();i++) {
        waveDisplayY+=getRowheightpx(i);
    }
    waveDisplayH=0;
    for(int i=getScalerow();i<getNrows();i++) {
        waveDisplayH+=getRowheightpx(i);
    }
    waveDisplayH/=2;
    waveDisplayY+=waveDisplayH;

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
    if(i<nrows_max) {
        return rowheightpx[i];
    } else {
        return 0;
    }
}

int LayoutModel::getNseg(int i) const
{
    if(i<nrows_max) {
        return nseg[i];
    } else {
        return 0;
    }
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
    if(i>=0 && i<nsegs_max) {
        return segwidth[i];
    } else {
        return 0;
    }
}

int LayoutModel::getCtly(int i) const
{
    if(i<=nsegs_max) {
        return ctly[i];
    } else {
        return 0;
    }
}

int LayoutModel::getChan(int i) const
{
    if(i>=0 && i<nsegs_max) {
        return chan[i];
    } else {
        return 0;
    }
}

int LayoutModel::getCtlx(int i) const
{
    if(i>=0 && i<nsegs_max) {
        return ctlx[i];
    } else {
        return 0;
    }
}

float LayoutModel::getFreq(int i) const
{
    if(i>=0 && i<nsegs_max) {
        return freq[i];
    } else {
        return 0;
    }
}

int LayoutModel::getMidinote(int i) const
{
    if(i>=0 && i<nsegs_max) {
        return midinote[i];
    } else {
        return 0;
    }
}

int LayoutModel::getPitch(int i) const
{
    if(i>=0 && i<nsegs_max) {
        return pitch[i];
    } else {
        return 0;
    }
}

int LayoutModel::getSegwidthpx(int i) const
{
    if(i>=0 && i<nsegs_max) {
        return segwidthpx[i];
    } else {
        return 0;
    }
}

int LayoutModel::getSegwidthmax(int i) const
{
    if(i<nrows_max) {
        return segwidthmax[i];
    } else {
        return 0;
    }
}

int LayoutModel::getPressed(int i) const
{
    if(i>=0 && i<nsegs_max) {
        return pressed[i];
    } else {
        return 0;
    }
}

int LayoutModel::getSegtype(int i) const
{
    if(i>=0 && i<nsegs_max) {
        return segtype[i];
    } else {
        return 0;
    }
}

QString *LayoutModel::getSegText(int i) const
{
    if(i>=0 && i<nsegs_max) {
        return &(segText[i]);
    } else {
        return NULL;
    }
}

int LayoutModel::getSegH(int i) const
{
    if(i>=0 && i<nsegs_max) {
        return segH[i];
    } else {
        return 0;
    }
}

void LayoutModel::incPressed(int i)
{
    if(i>=0 && i<nsegs_max) {
        pressed[i]++;
    }
}

void LayoutModel::decPressed(int i)
{
    if(i>=0 && i<nsegs_max) {
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
    if(i>=0 && i<nsegs_max) {
        return xrel[i];
    } else {
        return 0;
    }
}

void LayoutModel::setXrel(int i,float value)
{
    if(i>=0 && i<nsegs_max && value>=0) {
        xrel[i] = value;
    }
}

float LayoutModel::getYrel(int i) const
{
    if(i>=0 && i<nsegs_max) {
        return yrel[i];
    } else {
        return 0;
    }
}

void LayoutModel::setYrel(int i,float value)
{
    if(i>=0 && i<nsegs_max && value>=0) {
        yrel[i] = value;
    }
}

int LayoutModel::getXrelq(int i) const
{
    if(i>=0 && i<nsegs_max) {
        return xrelq[i];
    } else {
        return 0;
    }
}

void LayoutModel::setXrelq(int i, int value)
{
    if(i>=0 && i<nsegs_max) {
        xrelq[i] = value;
    }
}

int LayoutModel::getYrelq(int i) const
{
    if(i>=0 && i<nsegs_max) {
        return yrelq[i];
    } else {
        return 0;
    }
}

void LayoutModel::setYrelq(int i, int value)
{
    if(i>=0 && i<nsegs_max) {
        yrelq[i] = value;
    }
}

float LayoutModel::getSoundParam(int i) const
{
    if(i<soundmem.soundparam_max) {
        return soundmem.soundParam[progmem.progmem[actProgmem].sound][i];
    } else {
        return 0;
    }
}

int LayoutModel::getSoundParamMax() const
{
    return soundmem.soundparam_max;
}

void LayoutModel::setSoundParam(int i, float value)
{
    if(i<soundmem.soundparam_max) {
        soundmem.soundParam[progmem.progmem[actProgmem].sound][i]=value;
    }
}

int LayoutModel::getSound()
{
    return progmem.progmem[actProgmem].sound;
}

void LayoutModel::setSound(int v)
{
    progmem.progmem[actProgmem].sound=v;
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

void LayoutModel::setFreq(int i, float f)
{
    if(i>=0 && i<nsegs_max) {
        if(f>=1.0f && f<=freq_max) {
            freq[i]=f;
            midinote[i]=round(Log2(f/(freq_a/64.0))*12.0-3);
            pitch[i]=calcPitch(midinote[i],f);
            segH[i]=note2hue(midinote[i]);
            //qDebug() << "LayoutModel::setFreq " << f << " midi " << midinote[i] << " pitch " << pitch[i];
        }
    }
}

float LayoutModel::calcPitch(int midinote, float f) {
    return round(Log2(f/midi2fequal[midinote])*12*8192/2);
}

void LayoutModel::setMidinote(int i, int v)
{
    if(i>=0 && i<nsegs_max) {
        if(v<=midinote_max) {
            midinote[i]=v;
            freq[i]=midi2f[v];
            pitch[i]=0;
            segH[i]=note2hue(v);
        }
    }
}

void LayoutModel::setSegtype(int i, int v) const
{
    if(i>=0 && i<nsegs_max) {
        if(v==1) {
            // for segtype 1 (transistion area)
            // make sure, both neighbours are 0 (play area)
            if(i>0 && i<nsegs_max-1) {
                if(segtype[i+1]==0 && segtype[i-1]==0) {
                    segtype[i]=v;
                }
            }
        } else {
            segtype[i]=v;
        }
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
    if(i>=0 && i<nsegs_max) {
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
    if(i>=0 && i<nsegs_max) {
        if(v>=0 && v<16) {
            chan[i]=v;
        }
    }
}

void LayoutModel::setCtlx(int i, int v) const
{
    if(i>=0 && i<nsegs_max) {
        if(v>=0 && v<=127) {
            ctlx[i]=v;
        }
    }
}

void LayoutModel::setCtly(int i, int v) const
{
    if(i>=0 && i<nsegs_max) {
        if(v>=0 && v<=127) {
            ctly[i]=v;
        }
    }
}

void LayoutModel::setPressed(int i, int v) const
{
    if(i>=0 && i<nsegs_max) {
        pressed[i]=v;
    }
}

void LayoutModel::setSegtext(int i, QString t) const
{
    if(i>=0 && i<nsegs_max) {
        segText[i]=t;
    }
}

int LayoutModel::getBasenote() const
{
    return progmem.progmem[actProgmem].basenote;
}

void LayoutModel::setBasenote(int v)
{
    if(v<=midinote_max) {
        progmem.progmem[actProgmem].basenote = v;
    }
}

bool LayoutModel::getBscale(int n)
{
    if(n<progmem.bscale_max) {
        return progmem.progmem[actProgmem].bscale[n];
    } else {
        return false;
    }
}

void LayoutModel::setBscale(int n, bool v)
{
    if(n<progmem.bscale_max) {
        progmem.progmem[actProgmem].bscale[n]=v;
    }
}

int LayoutModel::getTopoct() const
{
    return progmem.progmem[actProgmem].topoct;
}

void LayoutModel::setTopoct(int v)
{
    if(v>10) {
        v=10;
    }
    if(progmem.progmem[actProgmem].baseoct>v) {
        progmem.progmem[actProgmem].topoct=progmem.progmem[actProgmem].baseoct;
        progmem.progmem[actProgmem].baseoct=v;
    } else {
        progmem.progmem[actProgmem].topoct = v;
    }
}

int LayoutModel::getBaseoct() const
{
    return progmem.progmem[actProgmem].baseoct;
}

void LayoutModel::setBaseoct(int v)
{
    if(v>10) {
        v=10;
    }
    if(progmem.progmem[actProgmem].topoct<v) {
        progmem.progmem[actProgmem].baseoct=progmem.progmem[actProgmem].topoct;
        progmem.progmem[actProgmem].topoct=v;
    } else {
        progmem.progmem[actProgmem].baseoct=v;
    }
}

void LayoutModel::setRowsGen(int v)
{
    if(v>=0 && v<nsegs-scaleStartSeg) {
        //qDebug() << "setRowGen " << v;
        progmem.progmem[actProgmem].rows=v;
        updateLayout();
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
    if(i<=nrows_max) {
        rowheight[i]=v;
    }
}

void LayoutModel::setNrows(int v)
{
    if(v<=nrows_max && v>0) {
        nrows=v;
    }
}

void LayoutModel::setNseg(int i, int v)
{
    if(i<=nrows_max) {
        if(nsegs-nseg[i]+v<=nsegs_max) {
            nsegs-=nseg[i];
            nseg[i]=v;
            nsegs+=v;
        }
    }
}

void LayoutModel::setNsegs(int v)
{
    if(v<=nsegs_max) {
        nsegs=v;
    }
}

int LayoutModel::getScalerow() const
{
    return scaleRow;
}

int LayoutModel::getScaleStartSeg() const
{
    return scaleStartSeg;
}

void LayoutModel::setScalerow(int r)
{
    if(r>0 && r<nrows) {
        scaleRow=r;
    }
}

void LayoutModel::setScaleStartSeg(int s)
{
    if(s>0 && s<nsegs) {
        scaleStartSeg=s;
    }
}

int LayoutModel::note2hue(int note)
{
    float calccol=(float)((note+4)%12)*30;
    return (int)calccol%360;
}

float LayoutModel::midi2freq(int note)
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
    float p=(x+3)%12;
    int q=x%12;
    return (freq_a / 64.0f) * (pow(2.0 , (double)((float)oct*1200.0f+(midi2fcent[q]+100.0f*p)) / 1200.0));
}

float LayoutModel::calcMidi2Fequal(int x)
{
    int oct=(x+3)/12;
    float p=(x+3)%12;
    return (freq_a / 64.0f) * (pow(2.0 , (double)((float)oct*1200.0f+100.0f*p) / 1200.0f));
}

void LayoutModel::calcMidi2f()
{
    for(int i=0;i<=midinote_max;i++) {
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

void LayoutModel::setMidi2fcent(int pos, float value)
{
    if(pos<12) {
        //qDebug() << "setMidi2fcent" << pos  << " " << value;
        midi2fcent[pos]=value;
    }
    calcMidi2f();
}

void LayoutModel::initMidi2f(int n)
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
    setEdit(!editMode);
}

void LayoutModel::setEdit(bool to)
{
    editMode=to;
    if(editMode) {
        resetLayout();
    } else {
        rowheight[scaleRow-1]=10;
        rowheight[scaleRow]=rowheightmax-10;
        editMode=false;
        for(int i=0;i<scaleRow-1;i++) {
            rowheight[i]=0;
        }
    }
    calcGeo();
    updateLayout();
}


void LayoutModel::resetLayout(int i) {
    currLayout=i;
    if(i==0) {
        scaleRow=0;
        scaleStartSeg=0;
        updateLayout();
    } else {
        QString filename;
        filename.sprintf(confpath,i);
        resetLayout(filename);
    }
}

void LayoutModel::resetLayout(QString filename) {
    layxml.readXml(filename);
}

void LayoutModel::resetLayout() {
    resetLayout(currLayout);
}

void LayoutModel::saveLayout() {
    QString filename;
    filename.sprintf(confpath,currLayout);
    layxml.writeXml(filename);
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
    return midi2TextEU[midi%12];
}

QString LayoutModel::getMidi2TextUrl(int midi) const
{
    return midi2TextUrl[midi%12];
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
                if(progmem.progmem[actProgmem].basenote==ctlx[seg]) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            } else if(ctly[seg]==-2) {
                note=progmem.progmem[actProgmem].basenote+ctlx[seg]+1;
                if(progmem.progmem[actProgmem].bscale[ctlx[seg]]) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            } else if(ctly[seg]==-3) {
                note=ctlx[seg]%12;
                yrel[seg]=midi2fcent[note%12]/200+0.5;
//                yrel[seg]=midi2fcent[(note+3)%12]/200+0.5;
            }
            int oct=progmem.progmem[actProgmem].baseoct+progmem.progmem[actProgmem].topoct;
            oct/=2;
            oct*=12;
            midinote[seg]=note+oct;
            freq[seg]=midi2f[note+oct];
            pitch[seg]=midi2fcent[note%12]*4096.0f/100.0f;
//            pitch[seg]=midi2fcent[(note+3)%12]*4096.0f/100.0f;
            segH[seg]=note2hue(note);
            segText[seg]=midi2TextEU[note%12];
        } else if(segtype[seg]==2 ) {
            if(ctly[seg]==-2) {
                if(actProgmem==ctlx[seg]) {
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
            } else if(ctly[seg]==-5) {
                segText[seg]=progmem.adr;
            } else if(ctly[seg]==-6) {
                segText[seg].sprintf("%d",progmem.port);
            }
        } else if(segtype[seg]==3 ) {
            // toggle buttons
            if(ctly[seg]==-2) {
                if(transMode) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            } else if(ctly[seg]==-3) {
                if(seqPlay) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            } else if(ctly[seg]==-4) {
                if(seqRec) {
                    pressed[seg]=1;
                } else {
                    pressed[seg]=0;
                }
            }
        } else if(segtype[seg]==4 ) {
            if(ctly[seg]==-2) {
                midinote[seg]=actProgmem/ctlx[seg];
                midinote[seg]*=ctlx[seg];
                xrelq[seg]=actProgmem - midinote[seg];
            } else if(ctly[seg]==-3) {
                int actSound=progmem.progmem[actProgmem].sound;
                midinote[seg]=actSound/ctlx[seg];
                midinote[seg]*=ctlx[seg];
                xrelq[seg]=actSound - midinote[seg];
            } else if(ctly[seg]==-4) {
                xrelq[seg]=progmem.progmem[actProgmem].topoct;
            } else if(ctly[seg]==-5) {
                xrelq[seg]=progmem.progmem[actProgmem].baseoct;
            } else if(ctly[seg]==-6) {
                xrelq[seg]=progmem.senderType;
            } else if(ctly[seg]==-7) {
                xrelq[seg]=progmem.channel;
            } else if(ctly[seg]==-8) {
                xrelq[seg]=progmem.errCorr;
            } else if(ctly[seg]==-9) {
                xrelq[seg]=progmem.progmem[actProgmem].rows;
            } else if(ctly[seg]==-13) {
                int actSeq=progmem.progmem[actProgmem].seq;
                midinote[seg]=actSeq/ctlx[seg];
                midinote[seg]*=ctlx[seg];
                xrelq[seg]=actSeq - midinote[seg];
            }
        } else if(segtype[seg]==5 ) {
            if(ctly[seg]>=102) {
                float soundparam=soundmem.soundParam[progmem.progmem[actProgmem].sound][ctly[seg]-102];
                yrelq[seg]=soundparam*(float)ctlx[seg];
                yrel[seg]=soundparam;
                //qDebug() << "updateLayout setting soundParam " << soundparam;
            }
        }
    }
    if(progmem.progmem[actProgmem].rows==0) {
        seg=generateScale(seg,true);
    } else {
        seg=generateScale(seg,false);
    }

    nrows=scaleRow+progmem.progmem[actProgmem].rows+1;
    int scalelen=seg-scaleStartSeg;

    int segsperrow=scalelen/(progmem.progmem[actProgmem].rows+1);
    int leftover=scalelen%(progmem.progmem[actProgmem].rows+1);
    int rowheightsum=0;
    for(int i=0;i<scaleRow;i++) {
        rowheightsum+=rowheight[i];
    }
    int rowheightcalc=(rowheightmax-rowheightsum)/(progmem.progmem[actProgmem].rows+1);

    //qDebug() << "rowheightcalc " <<rowheightcalc << " rowheightmax " << rowheightmax << " rowheightsum " << rowheightsum;
    //qDebug() << "scalelen " << scalelen << " segsperrow " << segsperrow << " leftover " << leftover << " scaleRow " << scaleRow << " nrows " << nrows;
    for(int i=0;i<progmem.progmem[actProgmem].rows+1;i++) {
        nseg[scaleRow+i]=segsperrow;
        segwidthmax[scaleRow+i]=segsperrow;
        rowheight[scaleRow+i]=rowheightcalc;
    }
    if(segsperrow*(progmem.progmem[actProgmem].rows+1)>scalelen) {
        nseg[scaleRow+progmem.progmem[actProgmem].rows]=leftover;
        segwidthmax[scaleRow+progmem.progmem[actProgmem].rows]=leftover;
    } else {
        nseg[scaleRow+progmem.progmem[actProgmem].rows]+=leftover;
        segwidthmax[scaleRow+progmem.progmem[actProgmem].rows]+=leftover;
    }
    //nseg[scaleRow]=seg-scaleStartSeg;
    //segwidthmax[scaleRow]=seg-scaleStartSeg;
    nsegs=seg;
    //qDebug() << "nsegs " << nsegs << " nrows " << nrows;
    calcGeo(widthPx,heightPx);
    //qDebug() << "nsegs " << nsegs << " nrows " << nrows;
}

int LayoutModel::generateScale(int seg, bool firstlast) {
    // row 4: the scale
    //seg=scaleStartSeg;
    if(seg>=nsegs_max) return seg;
    int startseg=seg; // save startseg to count added segs

    for(int i=progmem.progmem[actProgmem].baseoct;i<=progmem.progmem[actProgmem].topoct;i++) {
        int calcnote=progmem.progmem[actProgmem].basenote+i*12;
        midinote[seg]=calcnote;
        freq[seg]=midi2f[calcnote];
        //pitch[seg]=midi2fcent[(calcnote+3)%12]*4096.0f/100.0f;
        pitch[seg]=midi2fcent[calcnote%12]*4096.0f/100.0f;
        QString octnum;
        octnum.sprintf("%d",i+1);
        segText[seg]=midi2TextEU[calcnote%12];
        segText[seg].append(octnum);
        ctlx[seg]=0;
        ctly[seg]=1;
        segwidth[seg]=1;
        segtype[seg]=0;
        segH[seg]=note2hue(calcnote);
        pressed[seg]=0;
        for(int j=0;j<11;j++) {
            if(progmem.progmem[actProgmem].bscale[j]) {
                if(transMode) {
                    seg++;
                    if(seg>=nsegs_max) return seg;
                    segtype[seg]=1;
                    segText[seg]="";
                    segwidth[seg]=1;
                    ctlx[seg]=0;
                    ctly[seg]=1;
                    pressed[seg]=0;
                }
                seg++;
                if(seg>=nsegs_max) return seg;
                segtype[seg]=0;
                segwidth[seg]=1;
                int thisnote=calcnote+j+1;
                midinote[seg]=thisnote;
                freq[seg]=midi2f[thisnote];
                //pitch[seg]=midi2fcent[(thisnote+3)%12]*4096.0f/100.0f;
                pitch[seg]=midi2fcent[thisnote%12]*4096.0f/100.0f;
                //qDebug() << "pitch " << pitch[seg] << " " ;
                segText[seg]=midi2TextEU[thisnote%12];
                segText[seg].append(octnum);
                ctlx[seg]=0;
                ctly[seg]=1;
                segH[seg]=note2hue(thisnote);
                pressed[seg]=0;
            }
        }
        seg++;
        if(seg>=nsegs_max) return seg;
        if(i<progmem.progmem[actProgmem].topoct && transMode) {
            segtype[seg]=1;
            segText[seg]="";
            segwidth[seg]=1;
            pressed[seg]=0;
            ctlx[seg]=0;
            ctly[seg]=1;
            seg++;
            if(seg>=nsegs_max) return seg;
        }
    }
    // it sounds nice if an octave finishes
    // on top with the first note
    //
    if(firstlast==true) {
        // but only, if there is more than 1 seg
        if(seg>startseg+1) {
            
            // is there enough space in transMode?
            if(transMode) {
                if(seg+1<nsegs_max) {
                    segtype[seg]=1;
                    segText[seg]="";
                    segwidth[seg]=1;
                    pressed[seg]=0;
                    ctlx[seg]=0;
                    ctly[seg]=1;
                    seg++;
                }
            }
            
            // draw top segment only
            // in case that we're  not at the really upper end
            // border (nsegsmax) of the seg storage
            if(!(transMode && seg+1>nsegs_max)) {
                int calcnote=progmem.progmem[actProgmem].basenote+(progmem.progmem[actProgmem].topoct+1)*12;
                midinote[seg]=calcnote;
                freq[seg]=midi2f[calcnote];
                segText[seg]=midi2TextEU[calcnote%12];
                segwidth[seg]=1;
                segtype[seg]=0;
                ctlx[seg]=0;
                ctly[seg]=1;
                segH[seg]=note2hue(calcnote);
                pressed[seg]=0;
                seg++;
            }
        }
    }
    return seg;
}

void LayoutModel::setDisplayAddress(QString adr)
{
    progmem.adr=adr;
}

void LayoutModel::setDisplayPort(int port)
{
    progmem.port=port;
}

void LayoutModel::setSeq(int s)
{
    progmem.progmem[actProgmem].seq = s;
}


/*
void LayoutModel::setSegH(int i, int v)
{
    if(i>=0 && i<nsegs_max) {
        segH[i]=v;
    }
}
 */

void LayoutModel::setAllCtlx(int v)
{
    setAll(nsegs_max,ctlx,v);
}

void LayoutModel::setAllCtly(int v)
{
    setAll(nsegs_max,ctly,v);
}

void LayoutModel::setAllChan(int v)
{
    setAll(nsegs_max,chan,v);
}

void LayoutModel::setActProgmem(int n)
{
    // restore new setup
    if(n>=0 && n<progmem.progmem_max) {
        actProgmem=n;
        updateLayout();
    }
}

bool LayoutModel::getEditMode() const
{
    return editMode;
}

void LayoutModel::readProgmemXml(QString filename)
{
    progmem.readProgmemXml(filename+"prog.xml");
    soundmem.readSoundmemXml(filename+"sound.xml");
}

void LayoutModel::writeProgmemXml(QString filename)
{
    progmem.writeProgmemXml(filename+"prog.xml");
    soundmem.writeSoundmemXml(filename+"sound.xml");
}
