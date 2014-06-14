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

#include "senderoscxy.h"
#include "../comm/libofqf/qoscclient.h"

SenderOscXY::SenderOscXY(RC1 *rc1)
{
    oscout=new QOscClient(QHostAddress("255.255.255.255"),3334);
    oscout->setAddress(QHostAddress("255.255.255.255"),3334);
    this->rc1=rc1;
    x=0;
    y=0;
}

void SenderOscXY::noteOn(int voiceId, double fr, int midinote, int pitch, int vel)
{
}

void SenderOscXY::noteOff(int voiceId, double f, int midinote, int pitch, int vel)
{
}

void SenderOscXY::setDestination(QHostAddress a, int p)
{
    oscout->setAddress(a,p);
}

void SenderOscXY::pc(int v1)
{

}

void SenderOscXY::cc(int voiceId, int cc, double v1)
{

    if(cc==1) x=v1;
    if(cc==2) y=v1;

    QVariantList v;
    v.append(x);
    v.append(y);
    sendOsc("/xy",v);
}

void SenderOscXY::sendOsc(QString path, QVariant list)
{
//    qDebug() << " sendOsc to " << path << " values " << list;
    oscout->sendData(path,list);
    rc1->getEvstat()->incOsccount();
}
