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
#include "senderoscpuredata.h"
#include "comm/libofqf/qoscclient.h"

SenderOscPuredata::SenderOscPuredata(RC1 *rc1)
{
    oscout=new QOscClient(QHostAddress("255.255.255.255"),3334);
    oscout->setAddress(QHostAddress("255.255.255.255"),3334);
    notestate=new quint8[127];
    for(int i=0;i<127;i++) {
        notestate[i]=0;
    }
    prog=0;
    onNoteCnt=0;
    this->rc1=rc1;
}

void SenderOscPuredata::note(int c, int voiceId, int f, int vel)
{
    if(vel>0) {
        if(notestate[f]==0) {
            notestate[f]=vel;
            QVariantList v;
            v.append(f);
            v.append(vel);
            v.append(c);
            sendOsc("/note",v);
            onNoteCnt++;
            rc1->getEvstat()->incOscnoteoncount();
        } else {
            rc1->getEvstat()->incAlreadyoncount();
        }
    } else {
        notestate[f]=0;
        QVariantList v;
        v.append(f);
        v.append(0);
        v.append(c);
        sendOsc("/note",v);
        onNoteCnt--;
        rc1->getEvstat()->incOscnoteoffcount();
//  paranoia option, make sure it is really all off
//        if(onNoteCnt==0) sendOsc("/alloff",1);
    }
}

void SenderOscPuredata::pc(int c, int v1)
{
    QVariantList v;
    v.append(c);
    v.append(v1);
    sendOsc("/pc",v);
}

void SenderOscPuredata::cc(int c, int voiceId, int cc, double v1)
{
    // translate value to midi
    int v1mid=(double)127*v1;
    // translate cc numbers
    QVariantList v;
    v.append(cc);
    v.append(v1mid);
    v.append(c);
    sendOsc("/cc",v);
}

void SenderOscPuredata::sendOsc(QString path, QVariant list)
{
//    qDebug() << " sendOsc to " << path << " values " << list;
    oscout->sendData(path,list);
    rc1->getEvstat()->incOsccount();
}
