#include "sendermulti.h"

SenderMulti::SenderMulti(RC1 * rc1)
{
    s1=new SenderMobileSynth(rc1);
    s2=new SenderOscPuredata(rc1);

}

void SenderMulti::cc(int c, int voiceId, int cc, double v1)
{
    s1->cc(c,voiceId,cc,v1);
    s2->cc(c,voiceId,cc,v1);
}

void SenderMulti::pc(int c, int v1)
{
    s1->pc(c,v1);
    s2->pc(c,v1);
}

void SenderMulti::note(int c, int voiceId, double f, int v)
{
    s1->note(c,voiceId,f,v);
    s2->note(c,voiceId,f,v);
}

void SenderMulti::pitch(int c, int voiceId, double f)
{
    s1->pitch(c,voiceId,f);
    s2->pitch(c,voiceId,f);
}

void SenderMulti::setDestination(QHostAddress a, int p)
{
    s1->setDestination(a,p);
    s2->setDestination(a,p);
}
