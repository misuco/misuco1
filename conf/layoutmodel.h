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
public:
    LayoutModel();
    void calcGeo(int w, int h);

    int getWidth() const;
    int getHeight() const;

    int getNrows() const;

    int getRowheightpx(int i) const;
    int getNseg(int i) const;
    int getNsegs() const;
    int getSegwidth(int i) const;
    int getSegwidthmax(int i) const;
    int getSegwidthpx(int i) const;
    double getNote(int i) const;
    int getCtlx(int i) const;
    int getCtly(int i) const;
    int getChan(int i) const;
    int getPressed(int i) const;
    int getSegtype(int i) const;
    QString * getSegText(int i) const;
    int getSegH(int i) const;
    void incPressed(int i);
    void decPressed(int i);
    void setXY(int x, int y);
    void setScale(int start, int n, int step, bool withTransistion);
    void setSegH(int i, int v);
    void setAllCtlx(int v);
    void setAllCtly(int v);

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
    int *segtype;       // segmenttyp: 0=note, 1=transition
    double *note;
    double *midi2f;
    int *ctlx;
    int *ctly;
    int *chan;
    int *pressed;
    QString * segText;
    int * segH;
    int width;
    int height;

    void setAll(int n, int * d, int v);

};

#endif // LAYOUTMODEL_H
