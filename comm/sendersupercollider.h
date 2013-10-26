#ifndef SENDERSUPERCOLLIDER_H
#define SENDERSUPERCOLLIDER_H

#include "rc1.h"
#include "isender.h"
#include "comm/libofqf/qoscclientinterface.h"

class SenderSuperCollider : public ISender
{
public:
    SenderSuperCollider(RC1 * rc1);
    virtual void cc(int c, int voiceId, int cc, double v1);
    virtual void pc(int c, int v1);
    virtual void note(int c, int voiceId, double f, int vel);
    virtual void setDestination(QHostAddress a,int p);

private:
    QOscClientInterface* oscout;
    RC1 * rc1;
    quint8 onNoteCnt;   // count of on Notes
    void sendOsc(QString path, QVariant list);
};

#endif // SENDERSUPERCOLLIDER_H
