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
#include "senderoscmidigeneric.h"
#include "../comm/libofqf/qoscclient.h"

SenderOscMidiGeneric::SenderOscMidiGeneric()
{
    adr="255.255.255.255";
    port=3150;
    oscout=new QOscClient();
    oscout->setAddress(adr,port);
    notestate=new quint8[1024];
    ccstate=new int[1024];
    for(int i=0;i<1024;i++) {
        notestate[i]=0;
        ccstate[i]=0;
    }
    prog=0;
}

SenderOscMidiGeneric::~SenderOscMidiGeneric()
{
    delete(notestate);
    delete(ccstate);
    delete(oscout);
}

void SenderOscMidiGeneric::noteOn(int chan, int voiceId, float, int midinote, int pitch, int)
{
    //qDebug() << "SenderOscMidiGeneric::noteOn " << chan << " " << voiceId << " " << midinote << " " << pitch;
    int f = midinote;
    int vid=voiceId%1024;

    notestate[vid]=f;
    QVariantList v;
    v.append(f);
    v.append(127);
    v.append(chan);
    QString path;
    path.sprintf("/note/%d",chan);
    sendOsc(path,v);

    v.clear();
    v.append(pitch);
    v.append(chan);
    path.sprintf("/pitch/%d",chan);
    sendOsc(path,v);
}

void SenderOscMidiGeneric::noteOff(int chan, int, int midinote)
{
    QVariantList v;
    QString path;
    //v.append(notestate[voiceId%1024]);
    v.append(midinote);
    v.append(0);
    v.append(chan);
    path.sprintf("/note/%d",chan);
    sendOsc(path,v);
}

void SenderOscMidiGeneric::pitch(int chan, int voiceId, float, int midinote, int pitch)
{
    //qDebug() << "SenderOscMidiGeneric::pitch " << chan << " " << voiceId << " " << midinote << " " << pitch;
    QVariantList v;
    QString path;

    int f = midinote;
    int vid=voiceId%1024;

    if(notestate[vid]!=f) {
        v.append(notestate[vid]);
        v.append(0);
        v.append(chan);
        path.sprintf("/note/%d",chan);
        sendOsc(path,v);

        v.clear();
        v.append(f);
        v.append(127);
        v.append(chan);
        path.sprintf("/note/%d",chan);
        sendOsc(path,v);

        notestate[vid]=f;
        v.clear();
    }
    v.append(pitch);
    v.append(chan);
    path.sprintf("/pitch/%d",chan);
    sendOsc(path,v);
}

void SenderOscMidiGeneric::setDestination(char * a, int p)
{
    delete(a);
    adr=a;
    port=p;
    oscout->setAddress(adr,p);
}

void SenderOscMidiGeneric::reconnect()
{
    delete(oscout);
    oscout=new QOscClient();
    oscout->setAddress(adr,port);
}

void SenderOscMidiGeneric::pc(int chan, int v1)
{
    QVariantList v;
    QString path;
    v.append(v1);
    v.append(chan);
    path.sprintf("/pc/%d",chan);
    sendOsc(path,v);
}

void SenderOscMidiGeneric::cc(int chan, int, int cc, float v1)
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
        v.append(cc);
        v.append(chan);
        path.sprintf("/cc/%d/%d",chan,cc);
        sendOsc(path,v);
    }
}

void SenderOscMidiGeneric::sendOsc(QString path, QVariant list)
{
    //qDebug() << " sendOsc to " << path << " values " << list;
    oscout->sendData(path,list);
}
