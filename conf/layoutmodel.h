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
#ifndef LAYOUTMODEL_H
#define LAYOUTMODEL_H
#include <QString>
#include "progmem.h"
#include "layoutxml.h"

#ifndef LAYOUTXML_H
#define LAYOUTXML_H

#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class LayoutModel;
class LayoutXml
{
public:
    LayoutXml();
    void setLayoutModel(LayoutModel* l) {layout=l;}
    void readXml(QString filename);
    void writeXml(QString filename);

private:
    QXmlStreamWriter xml;
    QXmlStreamReader xmlr;
    LayoutModel * layout;
    void readLayout();
    void writeLayout();
};

#endif // LAYOUTXML_H

class LayoutModel
{
    friend class LayoutXml;
public:
    LayoutModel();
    
    const float freq_a=440;
    const uint nrows_max=32;
    const uint nsegs_max=32*32;       // size of segments memory
    const uint midinote_max=151;
    const float freq_max=calcMidi2Fequal(midinote_max); // 50175
    
    void calcGeo(uint w, uint h);
    void calcGeo();

    uint getWidth() const;
    uint getHeight() const;

    //layout
    uint getNrows() const;
    uint getRowheightpx(uint i) const;
    uint getRowheightmax() {return rowheightmax;}
    uint getRowheight(uint i) {return rowheight[i];}
    uint getNseg(uint i) const;
    uint getNsegs() const;
    uint getSegwidth(uint i) const;
    uint getSegwidthmax(uint i) const;
    uint getSegwidthpx(uint i) const;
    QString * getSegText(uint i) const;
    uint getSegH(uint i) const;

    // control
    float getFreq(uint i) const;
    uint getMidinote(uint i) const;
    uint getPitch(uint i) const;
    uint getCtlx(uint i) const;
    uint getCtly(uint i) const;
    uint getChan(uint i) const;
    uint getPressed(uint i) const;
    uint getSegtype(uint i) const;

    void incPressed(uint i);
    void decPressed(uint i);
    void setPressed(uint i, uint v) const;

    void setFreq(uint i, float f);
    void setMidinote(uint i, uint v);
    void setSegtype(uint i, uint v) const;
    void setRowheight(uint i, uint v) const;
    void setRowheightmax(uint v);
    void setSegwidth(uint i, uint v) const;
    void setSegwidthmax(uint i, uint v) const;
    void setChan(uint i, uint v) const;
    void setCtlx(uint i, uint v) const;
    void setCtly(uint i, uint v) const;
    void setSegtext(uint i, QString t) const;

    void setSegH(uint i, uint v);
    void setAllCtlx(uint v);
    void setAllCtly(uint v);
    void setAllChan(uint v);
    uint getBasenote() const;
    void setBasenote(uint v);
    uint getBasescale() const;
    void setBasescale(uint v);
    bool getBscale(uint n);
    void setBscale(uint n, bool v);
    uint getTopoct() const;
    void setTopoct(uint v);
    uint getBaseoct() const;
    void setBaseoct(uint v);
    void setTransMode(bool t);
    void setRowheightpx(uint i, uint v);
    void setRowheight(uint i, uint v);

    void setNrows(uint v);
    void setNseg(uint i,uint v);
    void setNsegs(uint v);
    uint getScalerow() const;
    void setScaleStartSeg(uint s);

    void updateLayout();

    uint note2hue(uint note);
    float midi2freq(uint note);
    void setMidi2fcent(uint pos, float freq);
    void initMidi2f(uint n);
    void toggleEdit();

    uint getFontsize() const;
    uint getFontsizeS() const;
    QString getFont() const;
    QString getMidi2Text(uint midi) const;
    QString getMidi2TextUrl(uint midi) const;
    double Log2(double n);

    float getXrel(uint i) const;
    void setXrel(uint i,float value);
    float getYrel(uint i) const;
    void setYrel(uint i,float value);

    uint getXrelq(uint i) const;
    void setXrelq(uint i, int value);
    uint getYrelq(uint i) const;
    void setYrelq(uint i, int value);

    uint getSoundParam(uint i) const;
    uint getSoundParamMax() const;
    void setSoundParam(uint i, uint value);

    uint getCurrLayout() const;
    void setCurrLayout(uint value);

    uint getSegBorder() const;

    void resetLayout(uint i);
    void resetLayout(QString filename);
    void resetLayout();

    void setActProgmem(uint n);

    bool getEditMode() const;

    void readProgmemXml(QString filename);
    void writeProgmemXml(QString filename);

    uint generateScale(uint seg);

private:
    // view and event parameters
    uint nrows;
    uint nsegs;          // total number of active segments

    uint *rowheight;
    uint rowheightmax;
    uint *rowheightpx;
    uint *nseg;          // segments per row
    uint *segwidth;
    uint *segwidthmax;
    uint *segwidthpx;
    uint *segtype;       // segmenttyp: 0=note, 1=transition, 2=layout
    float *freq;        // frequency
    uint *midinote;      // midi note 0...127
    int *pitch;         // midi pitch -8191/8192
    float *xrel;
    float *yrel;
    uint *xrelq;
    uint *yrelq;

    uint *ctlx;
    uint *ctly;
    uint *chan;
    uint *pressed;

    // pure view parameters
    uint widthPx;
    uint heightPx;
    uint fontsize;
    uint fontsizeS;
    QString font;
    QString * segText;
    uint * segH;         // segment color hue
    uint segBorder;

    // view help parameters
    uint currLayout;
    bool editMode;

    // scale generator parameters
    bool transMode;     // transistion areas between segments

    LayoutXml layxml;
    ProgMem progmem;
    uint actProgmen;

    // here we declare from which part the performance area starts
    uint scaleStartSeg;
    uint scaleRow;

    // lookup tables
    float *midi2fequal;  // equal tempered midi scale
    float *midi2fcent;   // microtuning scale
    float *midi2f;       // microtuned frequency scale
    QString * midi2TextEU;
    QString * midi2TextUrl;

    // util function
    void setAll(uint n, uint * d, uint v);
    float calcMidi2f(int x);
    void calcMidi2f();
    float calcMidi2Fequal(int x);
    void calcMidi2Fequal();
    float calcPitch(int midinote, float f);
};

#endif // LAYOUTMODEL_H
