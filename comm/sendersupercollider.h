#ifndef SENDERSUPERCOLLIDER_H
#define SENDERSUPERCOLLIDER_H

#include "rc1.h"
#include "isender.h"
#include "../comm/libofqf/qoscclientinterface.h"

class SenderSuperCollider : public ISender
{
public:
    SenderSuperCollider(RC1 * rc1);
    virtual void cc(int voiceId, int cc, double v1);
    virtual void pc(int v1);
    virtual void noteOn(int voiceId, double f, int midinote, int pitch, int vel);
    virtual void noteOff(int voiceId);
    virtual void pitch(int voiceId, double f, int midinote, int pitch);
    virtual void setDestination(QHostAddress a,int p);

private:
    QOscClientInterface* oscout;
    RC1 * rc1;
    quint8 onNoteCnt;   // count of on Notes
    void sendOsc(QString path, QVariant list);
};

#endif // SENDERSUPERCOLLIDER_H
