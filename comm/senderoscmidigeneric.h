#ifndef SENDEROSCMIDIGENERIC_H
#define SENDEROSCMIDIGENERIC_H

#include "rc1.h"
#include "isender.h"
#include "../comm/libofqf/qoscclientinterface.h"

class SenderOscMidiGeneric : public ISender
{
public:
    SenderOscMidiGeneric(RC1 * rc1);
    ~SenderOscMidiGeneric();
    virtual void cc(int chan, int voiceId, int cc, float v1);
    virtual void pc(int chan, int v1);
    virtual void noteOn(int chan, int voiceId, float f, int midinote, int pitch, int vel);
    virtual void noteOff(int chan, int voiceId);
    virtual void pitch(int chan, int voiceId, float fr, int midinote, int pitch);
    virtual void setDestination(QHostAddress a,int p);

private:
    QOscClientInterface* oscout;
    int prog;              // current program
    quint8 * notestate;   // currently played notes
    int * ccstate;       // current ccval;
    quint8 onNoteCnt;   // count of on Notes
    void sendOsc(QString path, QVariant list);
};

#endif // SENDEROSCMIDIGENERIC_H
