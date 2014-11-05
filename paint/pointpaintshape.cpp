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
#include "pointpaintshape.h"

/*


        C	X	Y	w	h	t	wt	ht  hue
 X	    0	1	2	3	4	5	6	7   8
 Y	    9	10	11	12	13	14	15  16  17
 radX	18	19	20	21	22	23  24  25  26
 radY	27	28	29	30	31  32  33  34  35
 penH	36	37	38	39  40  41  42  45  44
 penS	45	46	47  48  49  50  52  52  53
 penL	54	55  56  57  58  59  60  62  62
 penA	63  64	65	66	67	68	69	70	71
 brushH 72	73	74	75	76	77	78	79  80
 brushS	81	82	83	84	85	86	87  88  89
 brushL	90	91	92	93	94	95  96  97  98
 brushA	99	100	101	102	103 104 105 106 107
 rot    108	109	110	111 112 113 114 115 116
 shape	117 118 119 120 121 122 123 124 125

        C	X	Y	w	h	t	wt	ht
 X	    0	1	2	3	4	5	6	7
 Y	    8	9	10	11	12	13	14	15
 radX	16	17	18	19	20	21	22	23
 radY	24	25	26	27	28	29	30	31
 penH	32	33	34	35	36	37	38	39
 penS	40	41	42	43	44	45	46	47
 penL	48	49	50	51	52	53	54	55
 penA	56	57	58	59	60	61	62	63
 brushH	64	65	66	67	68	69	70	71
 brushS	72	73	74	75	76	77	78	79
 brushL	80	81	82	83	84	85	86	87
 brushA	88	89	90	91	92	93	94	95
 rot	96	97	98	99	100	101	102	103
 shape	104	105	106	107	108	109	110	111
 
*/


PointPaintShape::PointPaintShape()
{
    nparams=14*9; // rows*cols
    params=new float[nparams];
    for(int i=0;i<nparams;i++) {
        params[i]=0;
    }

    // init x/y
    params[1]=1;
    params[11]=1;

    // radius 5 constant
    params[18]=5;
    params[27]=5;

    // radius by time
    params[23]=80;
    params[32]=50;

    // color pen constant
    params[36]=255;
    params[45]=255;
    params[54]=255;
    params[63]=255;
    // fade pen out
    params[68]=-255;

    // color brush constant
    params[72]=0;
    params[81]=0;
    params[90]=0;
    params[99]=250;
    // fade brush out
    params[104]=-250;

    // rotation constan 45
    params[108]=45;
    // rotate once per lt
    params[113]=360;

    // shape constan 1 (rect)
    params[116]=1;

}

void PointPaintShape::setParam(quint16 n, float v)
{
    params[n]=v;
}

float PointPaintShape::getParamMax(quint16)
{
    return 0;
}

float PointPaintShape::getParamMin(quint16)
{
    return 1;
}

float PointPaintShape::getParam(quint16 n)
{
    return params[n];
}

int PointPaintShape::getParamCount()
{
    return nparams;
}

void PointPaintShape::paint(Point * point, RC1 * v, QPainter * pnt)
{
    float radX=calcRow(2,point,v);
    float radY=calcRow(3,point,v);
    quint8 hPen=calcRow(4,point,v);
    quint8 sPen=calcRow(5,point,v);
    quint8 lPen=calcRow(6,point,v);
    quint8 aPen=calcRow(7,point,v);
    quint8 hBrush=calcRow(8,point,v);
    quint8 sBrush=calcRow(9,point,v);
    quint8 lBrush=calcRow(10,point,v);
    quint8 aBrush=calcRow(11,point,v);
    qint16 rot=calcRow(12,point,v);
    quint8 shp=(int)calcRow(13,point,v)%3;
    pnt->setPen(QColor::fromHsl(hPen,sPen,lPen,aPen));
    pnt->setBrush(QColor::fromHsl(hBrush,sBrush,lBrush,aBrush));
    /*
    pnt->drawEllipse(calcRow(0,i,pan)-radX,calcRow(1,i,pan)-radY, radX*2, radY*2);
    */
    pnt->translate(calcRow(0,point,v),calcRow(1,point,v));
    pnt->rotate(rot);
    if(shp==0) {
        pnt->drawEllipse(-radX,-radY, radX*2, radY*2);
    } else if(shp==1) {
        pnt->drawRect(-radX,-radY, radX*2, radY*2);
    } else if(shp==2) {
        QPoint points[4] = {
            QPoint(-radX, radY),
            QPoint(radX, radY),
            QPoint(0, -radY),
            QPoint(-radX, radY)
        };
        pnt->drawPolygon(points,4);
    }
    pnt->resetTransform();
}

float PointPaintShape::calcRow(quint16 row, Point * pnt, RC1 * v) {
    quint16 i0=row*9;
    float ltn=(float)(v->getNow()-pnt->getT())/(float)v->getTtl();
    float res=params[i0];       // spalte 0: konstantanteil
    res+=params[i0+1]*pnt->getXn()*v->getLayout()->getWidth();
    res+=params[i0+2]*pnt->getYn()*v->getLayout()->getHeight();
    res+=params[i0+3]*v->getLayout()->getWidth();
    res+=params[i0+4]*v->getLayout()->getHeight();
    res+=params[i0+5]*ltn;
    res+=params[i0+6]*ltn*v->getLayout()->getWidth();
    res+=params[i0+7]*ltn*v->getLayout()->getHeight();
    res+=params[i0+8]*pnt->getHue();
    //qDebug() << " row " << row << " col " << col << " res " << res;
    return res;
}
