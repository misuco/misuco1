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

#include "sendermulti.h"
#include "sendermobilesynth.h"
#include "senderoscmidigeneric.h"
#include "sendersupercollider.h"
#include "senderreaktor.h"
#include "senderoscpuredata.h"
#include "senderoscxy.h"
#include "sendermobilesynth.h"

SenderMulti::SenderMulti()
{
    senders.append(new SenderOscMidiGeneric());
    senders.append(new SenderMobileSynth());
    senders.append(new SenderSuperCollider());
    repeatOff=0;
    onCnt=0;
    for(int i=0;i<256;i++) {
        midiOn[i]=false;
    }
}

SenderMulti::~SenderMulti()
{
    delAll();
}

void SenderMulti::cc(int chan, int voiceId, int cc, float v1)
{
    for(int i=0;i<senders.count();i++) {
        senders.at(i)->cc(chan,voiceId,cc,v1);
    }
}

void SenderMulti::pc(int chan, int v1)
{
    for(int i=0;i<senders.count();i++) {
        senders.at(i)->pc(chan,v1);
    }
}

void SenderMulti::noteOn(int chan, int voiceId, float f, int midinote, int pitch, int v)
{
    //qDebug() << "SenderDebug::noteOn(" << chan << "," << voiceId << "," << f << "," << midinote << "," << pitch << "," << v << ")";
    for(int i=0;i<senders.count();i++) {
        senders.at(i)->noteOn(chan,voiceId,f,midinote,pitch,v);
    }
    onCnt++;
    midiOn[midinote]=true;
}

void SenderMulti::noteOff(int chan, int voiceId, int midinote)
{
    //qDebug() << "SenderDebug::noteOff(" << chan << "," << voiceId << "," << midinote << ")";
    for(int i=0;i<senders.count();i++) {
        senders.at(i)->noteOff(chan,voiceId,midinote);
    }
    offRepeat * otr=new offRepeat;
    otr->voiceId=voiceId;
    otr->midinote=midinote;
    otr->chan=chan;
    otr->offSent=1;
    offToRepeat.append(otr);
    midiOn[midinote]=false;
    onCnt--;
}

void SenderMulti::pitch(int chan, int voiceId, float f, int midinote, int pitch)
{
    for(int i=0;i<senders.count();i++) {
        senders.at(i)->pitch(chan,voiceId,f,midinote,pitch);
    }
}

void SenderMulti::setDestination(char * a, int p)
{
    for(int i=0;i<senders.count();i++) {
        senders.at(i)->setDestination(a,p);
    }
}

void SenderMulti::del(int i) {
    if(i<senders.count()) {
        delete(senders.at(i));
        senders.removeAt(i);
    }
}

void SenderMulti::delAll(){
    for(int i=0;i<senders.count();i++) {
        delete(senders.at(i));
    }
    senders.clear();
}

void SenderMulti::create(SenderType i) {
    switch(i) {
        case REAKTOR:
            senders.append(new SenderReaktor());
            break;
        case SUPERCOLLIDER:
            senders.append(new SenderSuperCollider());
            break;
        case GENERIC:
            senders.append(new SenderOscMidiGeneric());
            break;
        case MIDI:
            senders.append(new SenderOscPuredata());
            break;
        case XY:
            senders.append(new SenderOscXY());
            break;
        case MOBILESYNTH:
            senders.append(new SenderMobileSynth());
            break;
            
    }
}

void SenderMulti::setDestination(int i, char * a,int p) {
    if(i<senders.count()) {
        senders.at(i)->setDestination(a,p);
    }
}

void SenderMulti::reconnect() {
    for(int i=0;i<senders.count();i++) {
        senders.at(i)->reconnect();
    }
}

int SenderMulti::getPort()
{
    if(senders.count()>0)
        return senders.at(0)->getPort();
    else
        return 0;
}

char *SenderMulti::getAddress()
{
    if(senders.count()>0)
        return senders.at(0)->getAddress();
    else
        return 0;
}

void SenderMulti::sendOff() {
    for(int j=offToRepeat.count();j>0;j--) {
        int i=j-1;
        if(offToRepeat.at(i)->offSent>=repeatOff) {
            delete(offToRepeat.at(i));
            offToRepeat.removeAt(i);
        } else {
            for(int j=0;j<senders.count();j++) {
                if(senders.at(j)->voiceBased()==true || midiOn[offToRepeat.at(i)->midinote]==false) {
                    senders.at(j)->noteOff(offToRepeat.at(i)->chan, offToRepeat.at(i)->voiceId, offToRepeat.at(i)->midinote);
                }
            }
            //qDebug() << "repeating off " << offToRepeat.at(i)->midinote << " for " << offToRepeat.at(i)->offSent;
            offToRepeat.at(i)->offSent++;
        }
    }
}

void SenderMulti::reset1(int x, char * adr, int port) {
    delAll();
    switch(x) {
    case 0:
        create(SenderMulti::GENERIC);
        break;
    case 1:
        create(SenderMulti::MIDI);
        break;
    case 2:
        create(SenderMulti::SUPERCOLLIDER);
        break;
    case 3:
        create(SenderMulti::REAKTOR);
        break;
    case 4:
        create(SenderMulti::XY);
        break;
    }
    setDestination(0,adr,port);
}

