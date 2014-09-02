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

class LayoutModel
{
    friend class layoutxml;
public:
    LayoutModel();
    void calcGeo(int w, int h);
    void calcGeo();

    int getWidth() const;
    int getHeight() const;

    //layout
    int getNrows() const;
    int getRowheightpx(int i) const;
    int getRowheightmax() {return rowheightmax;}
    int getRowheight(int i) {return rowheight[i];}
    int getNseg(int i) const;
    int getNsegs() const;
    int getSegwidth(int i) const;
    int getSegwidthmax(int i) const;
    int getSegwidthpx(int i) const;
    QString * getSegText(int i) const;
    int getSegH(int i) const;

    // control
    float getValue(int i) const;
    int getValueInt(int i) const;
    int getPitch(int i) const;
    int getCtlx(int i) const;
    int getCtly(int i) const;
    int getChan(int i) const;
    int getPressed(int i) const;
    int getSegtype(int i) const;

    void incPressed(int i);
    void decPressed(int i);
    void setPressed(int i, int v) const;

    void setValue(int i, float v);
    void setValueInt(int i, int v);
    void setSegtype(int i, int v) const;
    void setRowheight(int i, int v) const;
    void setRowheightmax(int v);
    void setSegwidth(int i, int v) const;
    void setSegwidthmax(int i, int v) const;
    void setChan(int i, int v) const;
    void setCtlx(int i, int v) const;
    void setCtly(int i, int v) const;
    void setSegtext(int i, QString t) const;

    void setSegH(int i, int v);
    void setAllCtlx(int v);
    void setAllCtly(int v);
    void setAllChan(int v);
    int getBasenote() const;
    void setBasenote(int value);
    int getBasescale() const;
    void setBasescale(int value);
    bool getBscale(int n);
    void setBscale(int n, bool value);
    int getTopoct() const;
    void setTopoct(int value);
    int getBaseoct() const;
    void setBaseoct(int value);
    void setTransMode(bool t);
    void setRowheightpx(int i, int v);
    void setRowheight(int i, int v);

    void setNrows(int v);
    void setNseg(int i,int v);
    void setNsegs(int v);
    int getScalerow() const;
    void setScaleStartSeg(int s);

    void updateLayout();

    int note2hue(int note);
    int midi2freq(int note);

    int getFontsize() const;
    QString getFont() const;
    QString getMidi2Text(int midi) const;
    QString getMidi2TextUrl(int midi) const;
    double Log2(double n);

private:
    int nrows;
    int nsegs;          // total number of active segments
    int nrowsmax;
    int nsegsmax;       // size of segments memory

    int *rowheight;
    int rowheightmax;
    int *rowheightpx;
    int *nseg;          // segments per row
    int *segwidth;
    int *segwidthmax;
    int *segwidthpx;
    int *segtype;       // segmenttyp: 0=note, 1=transition, 2=layout
    float *value;       // frequency
    int *valueint;      // midi note 0...127
    int *pitch;         // midi pitch -8191/8192

    int *ctlx;
    int *ctly;
    int *chan;
    int *pressed;
    QString * segText;

    // lookup tables
    float *midi2f;
    float *midi2fpure;
    QString * midi2TextEU;
    QString * midi2TextUrl;
    QString * midi2TextIN;
    QString * midi2TextDO;
    int * segH;         // segment color hue

    bool transMode;     // transistion areas between segments

    int widthPx;
    int heightPx;
    int fontsize;
    QString font;

    // scale generator parameters
    int basenote;
    int topoct;
    int baseoct;
    bool bscale[11];
    int scalewidth;
    int scaleheight;

    // here we declare the bscale-start
    //int bscaleStartSeg;
    //int bscaleRow;

    // here we declare from which part the scale keyboard starts
    int scaleStartSeg;
    int scaleRow;

/*    int * factoryScaleValues;
    int * factoryScaleStart;
    int * factoryScaleLen;
    int nFactoryScales;*/

    void setAll(int n, int * d, int v);

};

#endif // LAYOUTMODEL_H
