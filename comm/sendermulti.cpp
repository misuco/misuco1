#include "sendermulti.h"

SenderMulti::SenderMulti(RC1 * rc1)
{
    s1=new SenderMobileSynth(rc1);
    s2=new SenderOscPuredata(rc1);

}

void SenderMulti::cc(int voiceId, int cc, double v1)
{
    s1->cc(voiceId,cc,v1);
    s2->cc(voiceId,cc,v1);
}

void SenderMulti::pc(int v1)
{
    s1->pc(v1);
    s2->pc(v1);
}

void SenderMulti::noteOn(int voiceId, double f, int midinote, int pitch, int v)
{
    s1->noteOn(voiceId,f,midinote,pitch,v);
    s2->noteOn(voiceId,f,midinote,pitch,v);
}

void SenderMulti::noteOff(int voiceId)
{
    s1->noteOff(voiceId);
    s2->noteOff(voiceId);
}

void SenderMulti::pitch(int voiceId, double f, int midinote, int pitch)
{
    s1->pitch(voiceId,f,midinote,pitch);
    s2->pitch(voiceId,f,midinote,pitch);
}

void SenderMulti::setDestination(QHostAddress a, int p)
{
    s1->setDestination(a,p);
    s2->setDestination(a,p);
}
