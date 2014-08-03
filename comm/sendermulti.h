#ifndef SENDERMULTI_H
#define SENDERMULTI_H

#include "isender.h"
#include "sendermobilesynth.h"
#include "senderoscpuredata.h"

class SenderMulti : public ISender
{
public:
    SenderMulti(RC1 * rc1);
    virtual void cc(int chan, int voiceId, int cc, double v1);
    virtual void pc(int chan, int v1);
    virtual void noteOn(int chan, int voiceId, double f, int midinote, int pitch, int v);
    virtual void noteOff(int chan, int voiceId);
    virtual void pitch(int chan, int voiceId, double f, int midinote, int pitch);
    virtual void setDestination(QHostAddress a,int p);

private:
    ISender * s1;
    ISender * s2;
};

#endif // SENDERMULTI_H
