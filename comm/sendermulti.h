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

#ifndef SENDERMULTI_H
#define SENDERMULTI_H

#include "isender.h"
#include "sendermobilesynth.h"
#include "senderoscmidigeneric.h"
#include "sendersupercollider.h"

class SenderMulti : public ISender
{
public:
    SenderMulti(RC1 * rc1);
    ~SenderMulti();
    virtual void cc(int chan, int voiceId, int cc, float v1);
    virtual void pc(int chan, int v1);
    virtual void noteOn(int chan, int voiceId, float f, int midinote, int pitch, int v);
    virtual void noteOff(int chan, int voiceId);
    virtual void pitch(int chan, int voiceId, float f, int midinote, int pitch);
    virtual void setDestination(QHostAddress a,int p);

private:
    ISender * s1;
    ISender * s2;
    ISender * s3;
};

#endif // SENDERMULTI_H
