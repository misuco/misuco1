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


#define NSOUNDPARAM 10
#define NPROGMEM 11

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
    float getFreq(int i) const;
    int getMidinote(int i) const;
    int getPitch(int i) const;
    int getCtlx(int i) const;
    int getCtly(int i) const;
    int getChan(int i) const;
    int getPressed(int i) const;
    int getSegtype(int i) const;

    void incPressed(int i);
    void decPressed(int i);
    void setPressed(int i, int v) const;

    void setFreq(int i, float v);
    void setMidinote(int i, int v);
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
    void setBasenote(int v);
    int getBasescale() const;
    void setBasescale(int v);
    bool getBscale(int n);
    void setBscale(int n, bool v);
    int getTopoct() const;
    void setTopoct(int v);
    int getBaseoct() const;
    void setBaseoct(int v);
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
    int midi2freq(uint note);
    void setMidi2fcent(uint pos, float freq);
    void initMidi2f(uint n);
    void toggleEdit();

    int getFontsize() const;
    int getFontsizeS() const;
    QString getFont() const;
    QString getMidi2Text(int midi) const;
    QString getMidi2TextUrl(int midi) const;
    double Log2(double n);

    float getXrel(int i) const;
    void setXrel(int i,float value);
    float getYrel(int i) const;
    void setYrel(int i,float value);

    int getXrelq(int i) const;
    void setXrelq(int i, int value);
    int getYrelq(int i) const;
    void setYrelq(int i, int value);

    int getSoundParam(int i) const;
    void setSoundParam(int i, int value);

    int getCurrLayout() const;
    void setCurrLayout(int value);

    int getSegBorder() const;

    void resetLayout(int i);
    void resetLayout(QString filename);
    void resetLayout();

    void readProgmemXml(QString filename);
    void writeProgmemXml(QString filename);
    void setActProgmem(int n);

    bool getEditMode() const;

private:
    // view and event parameters
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
    float *freq;        // frequency
    int *midinote;      // midi note 0...127
    int *pitch;         // midi pitch -8191/8192
    float *xrel;
    float *yrel;
    int *xrelq;
    int *yrelq;

    int *ctlx;
    int *ctly;
    int *chan;
    int *pressed;

    // pure view parameters
    int widthPx;
    int heightPx;
    int fontsize;
    int fontsizeS;
    QString font;
    QString * segText;
    int * segH;         // segment color hue
    int segBorder;

    // view help parameters
    int currLayout;
    bool editMode;

    // scale generator parameters
    bool transMode;     // transistion areas between segments

    // program memory
    struct prog {
        int basenote;
        int baseoct;
        int topoct;
        bool bscale[11];
        int soundParam[NSOUNDPARAM];
    };

    prog progmem[NPROGMEM];

    int actProgmen;

    // here we declare from which part the performance area starts
    int scaleStartSeg;
    int scaleRow;

    // lookup tables
    float *midi2fequal;  // equal tempered midi scale
    float *midi2fcent;   // microtuning scale
    float *midi2f;       // microtuned frequency scale
    float freq_a;
    QString * midi2TextEU;
    QString * midi2TextUrl;

    // util function
    void setAll(int n, int * d, int v);
    float calcMidi2f(int x);
    void calcMidi2f();
    float calcMidi2Fequal(int x);
    void calcMidi2Fequal();
    float calcPitch(int midinote, float f);
};

#endif // LAYOUTMODEL_H
