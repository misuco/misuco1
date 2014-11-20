/*
 
 Copyright (C) 2014 by Claudio Zopfi, Zurich, Suisse, z@x21.ch
 
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

#include "scales.h"
#include <QDebug>
#include <QFile>
/*
 * Chords[0]=["Major","1,3,5","4,3","X,Xmaj,XM","3,5","5,4"];
 * Chords[1]=["m","1,3b,5","3,4","Xminor,Xmin","4,5","5,3"];
 * Chords[2]=["7","1,3,5,7b","4,3,3","X dominant 7, Xdom7","3,3,2"];
 * Chords[3]=["m7","1,3b,5,7b","3,4,3","Xminor7, Xmin7"];
 * Chords[4]=["maj7","1,3,5,7","4,3,4","X Major 7, XMaj7,XM7"];
 * Chords[5]=["dim","1,3b,5b","3,3","X diminished","3,6","6,3"];
 * Chords[6]=["dim7","1,3b,5b,6","3,3,3","X diminished 7"];
 * Chords[7]=["7b5","1,3,5b,7b","4,2,4","X dominant 7 flat fifth, X7-5"];
 * Chords[8]=["7#5","1,3,5#,7","4,4,2","X dominant 7 sharp fifth, X7+5"];
 * Chords[9]=["m7b5","1,3b,5b,7b","3,3,4","Xm(b7), Xminor7b5"];
 * Chords[10]=["7b9","1,3,5,7b,9b","4,3,3,3","X7-9, X7(add b9)"];
 * Chords[11]=["6","1,3,5,6","4,3,2","X Major 6, XM6, XMaj6","3,2,3","2,3,4"];
 * Chords[12]=["m6","1,3b,5,6","3,4,2","X minor 6, Xmin6","4,2,3","2,3,3"];
 * Chords[13]=["6add9","1,3,5,6,9","4,3,2,5","X Major 6 add 9, X6/9, X6(add9)"];
 * Chords[14]=["9","1,3,5,7b,9","4,3,3,4","X dominant 9, Xdom9, X7(add9)"];
 * Chords[15]=["m9","1,3b,5,7b,9","3,4,3,4","X minor 9, Xmin9, Cm7(add9)"];
 * Chords[16]=["maj9","1,3,5,7,9","4,3,4,3","X Major, XM9, XMaj7(add9)"];
 * Chords[17]=["add9","1,3,5,9","4,3,7","C added 9th"];
 * Chords[18]=["11","1,3,5,7b,9,11","4,3,3,4,3","X dominant 11, X7(add11)"];
 * Chords[19]=["m11","1,3b,5,7b,9,11","3,4,3,4,3","X minor 11, Xmin11"];
 * Chords[20]=["13","1,3,5,7b,9b,11","4,3,3,4,3,4","X dominant 13, X7(add13)"];
 * Chords[21]=["sus2","1,2,5","2,5","X suspended 2","5,5","5,2"];
 * Chords[22]=["sus4","1,4,5","5,2","X suspended 4","2,5","5,5"];
 * Chords[23]=["aug","1,3,5#","4,4","X augmented, X+, X+5, X(#5)","4,4","4,4"];var Scales=[];
 * Scales[0]=["Major","1,2,3,4,5,6,7","2,2,1,2,2,2"];
 * Scales[1]=["Minor","1,2,3b,4,5,6b,7b","2,1,2,2,1,2"]
 *
 */

Scales::Scales()
{
}

void Scales::readXml(QString filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
        //qDebug("cannot read file");
    }
    xmlr.setDevice(&file);
    if (xmlr.readNextStartElement()) {
        if (xmlr.name() == "misucoscales" && xmlr.attributes().value("version") == "1.0")
            readScales();
        else
            xmlr.raiseError(QObject::tr("The file is not a MISUCO version 1.0 file."));
    }
    file.close();
}

QString Scales::getName(int n)
{
    if(n<2048) {
        return scaleNames[n];
    } else {
        return "";
    }
}

void Scales::readScales() {
    while (xmlr.readNextStartElement()) {
//        qDebug() << "read element " << xmlr.name();
        if (xmlr.name() == "scale") {
            int index=xmlr.attributes().value("id").toString().toInt();
            scaleNames[index]=xmlr.attributes().value("name").toString();
            xmlr.skipCurrentElement();
        } else {
            xmlr.skipCurrentElement();
        }
    }
}
