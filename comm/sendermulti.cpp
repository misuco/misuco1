#include "sendermulti.h"


SenderMulti::SenderMulti(RC1 * rc1)
{
    s1=new SenderMobileSynth(rc1);
    s2=new SenderOscMidiGeneric(rc1);
    s3=new SenderSuperCollider(rc1);
}

SenderMulti::~SenderMulti()
{
    delete(s1);
    delete(s2);
    delete(s3);
}

void SenderMulti::cc(int chan, int voiceId, int cc, float v1)
{
    s1->cc(chan,voiceId,cc,v1);
    s2->cc(chan,voiceId,cc,v1);
    s3->cc(chan,voiceId,cc,v1);
}

void SenderMulti::pc(int chan, int v1)
{
    s1->pc(chan,v1);
    s2->pc(chan,v1);
    s3->pc(chan,v1);
}

void SenderMulti::noteOn(int chan, int voiceId, float f, int midinote, int pitch, int v)
{
    s1->noteOn(chan,voiceId,f,midinote,pitch,v);
    s2->noteOn(chan,voiceId,f,midinote,pitch,v);
    s3->noteOn(chan,voiceId,f,midinote,pitch,v);
}

void SenderMulti::noteOff(int chan, int voiceId)
{
    s1->noteOff(chan,voiceId);
    s2->noteOff(chan,voiceId);
    s3->noteOff(chan,voiceId);
}

void SenderMulti::pitch(int chan, int voiceId, float f, int midinote, int pitch)
{
    s1->pitch(chan,voiceId,f,midinote,pitch);
    s2->pitch(chan,voiceId,f,midinote,pitch);
    s3->pitch(chan,voiceId,f,midinote,pitch);
}

void SenderMulti::setDestination(QHostAddress a, int p)
{
    s1->setDestination(a,p);
    s2->setDestination(a,p);
    s3->setDestination(a,57110);
}
