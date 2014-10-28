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
