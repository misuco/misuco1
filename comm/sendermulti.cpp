#include "sendermulti.h"

SenderMulti::SenderMulti(RC1 * rc1)
{
    s1=new SenderMobileSynth(rc1);
    s2=new SenderOscPuredata(rc1);

}

void SenderMulti::cc(int chan, int voiceId, int cc, double v1)
{
    s1->cc(chan,voiceId,cc,v1);
    s2->cc(chan,voiceId,cc,v1);
}

void SenderMulti::pc(int chan, int v1)
{
    s1->pc(chan,v1);
    s2->pc(chan,v1);
}

void SenderMulti::noteOn(int chan, int voiceId, double f, int midinote, int pitch, int v)
{
    s1->noteOn(chan,voiceId,f,midinote,pitch,v);
    s2->noteOn(chan,voiceId,f,midinote,pitch,v);
}

void SenderMulti::noteOff(int chan, int voiceId)
{
    s1->noteOff(chan,voiceId);
    s2->noteOff(chan,voiceId);
}

void SenderMulti::pitch(int chan, int voiceId, double f, int midinote, int pitch)
{
    s1->pitch(chan,voiceId,f,midinote,pitch);
    s2->pitch(chan,voiceId,f,midinote,pitch);
}

void SenderMulti::setDestination(QHostAddress a, int p)
{
    s1->setDestination(a,p);
    s2->setDestination(a,p);
}
