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
#include "senderreaktor.h"
#include "../comm/libofqf/qoscclient.h"

SenderReaktor::SenderReaktor(RC1 *rc1)
{
    oscout=new QOscClient(QHostAddress("255.255.255.255"),3334);
    oscout->setAddress(QHostAddress("255.255.255.255"),3334);
    notestate=new quint8[1024];
    ccstate=new int[1024];
    for(int i=0;i<1024;i++) {
        notestate[i]=0;
        ccstate[i]=0;
    }
    prog=0;
    onNoteCnt=0;
}

SenderReaktor::~SenderReaktor()
{
    delete(notestate);
    delete(ccstate);
    delete(oscout);
}

void SenderReaktor::noteOn(int chan, int voiceId, float, int midinote, int pitch, int)
{
    int f = midinote;
    int vid=voiceId%1024;

    notestate[vid]=f;
    QVariantList v;
    v.append(f);
    v.append(127);
    QString path;
    path.sprintf("/note/%d",chan);
    sendOsc(path,v);

    v.clear();
    v.append(pitch);
    path.sprintf("/pitch/%d",chan);
    sendOsc(path,v);

    onNoteCnt++;
}

void SenderReaktor::noteOff(int chan, int voiceId)
{
    QVariantList v;
    QString path;
    v.append(notestate[voiceId%1024]);
    v.append(0);
    path.sprintf("/note/%d",chan);
    sendOsc(path,v);
    onNoteCnt--;
}

void SenderReaktor::pitch(int chan, int voiceId, float, int midinote, int pitch)
{
    QVariantList v;
    QString path;

    int f = midinote;
    int vid=voiceId%1024;

    if(notestate[vid]!=f) {
        v.append(notestate[vid]);
        v.append(0);
        path.sprintf("/note/%d",chan);
        sendOsc(path,v);

        v.clear();
        v.append(f);
        v.append(127);
        path.sprintf("/note/%d",chan);
        sendOsc(path,v);

        notestate[vid]=f;
    }
    v.append(pitch);
    path.sprintf("/pitch/%d",chan);
    sendOsc(path,v);
}

void SenderReaktor::setDestination(QHostAddress a, int p)
{
    oscout->setAddress(a,p);
}

void SenderReaktor::pc(int chan, int v1)
{
    QVariantList v;
    QString path;
    v.append(v1);
    path.sprintf("/pc/%d",chan);
    sendOsc(path,v);
}

void SenderReaktor::cc(int chan, int, int cc, float v1)
{
    //qDebug() <<  "SenderOscPuredata::cc " << cc << " v1 " << v1;

    // translate value to midi
    int v1mid=(float)127*v1;

    // translate cc numbers
    if(v1mid!=ccstate[cc]) {
        ccstate[cc]=v1mid;
        QVariantList v;
        QString path;
        v.append(v1mid);
        path.sprintf("/cc/%d/%d",chan,cc);
        sendOsc(path,v);
    }
}

void SenderReaktor::sendOsc(QString path, QVariant list)
{
    //qDebug() << " sendOsc to " << path << " values " << list;
    oscout->sendData(path,list);
}
