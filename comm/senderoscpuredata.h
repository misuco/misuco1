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
#ifndef SENDEROSCPUREDATA_H
#define SENDEROSCPUREDATA_H

#include "rc1.h"
#include "isender.h"
#include "comm/libofqf/qoscclientinterface.h"

class SenderOscPuredata : public ISender
{
public:
    SenderOscPuredata(RC1 * rc1);
    virtual void cc(int c, int voiceId, int cc, double v1);
    virtual void pc(int c, int v1);
    virtual void note(int c, int voiceId, int f, int vel);
private:
    QOscClientInterface* oscout;
    RC1 * rc1;

    int prog;           // current program
    quint8 * notestate;      // currently played notes
    quint8 onNoteCnt;   // count of on Notes

    void sendOsc(QString path, QVariant list);

};

#endif // SENDEROSCPUREDATA_H
