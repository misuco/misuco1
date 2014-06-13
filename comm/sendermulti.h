#ifndef SENDERMULTI_H
#define SENDERMULTI_H

#include "isender.h"
#include "sendermobilesynth.h"
#include "senderoscpuredata.h"

class SenderMulti : public ISender
{
public:
    SenderMulti(RC1 * rc1);
    virtual void cc(int c, int voiceId, int cc, double v1);
    virtual void pc(int c, int v1);
    virtual void note(int c, int voiceId, double f, int v);
    virtual void pitch(int c, int voiceId, double f);
    virtual void setDestination(QHostAddress a,int p);

private:
    ISender * s1;
    ISender * s2;
};

#endif // SENDERMULTI_H
