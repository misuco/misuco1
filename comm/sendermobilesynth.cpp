#include "sendermobilesynth.h"

#ifdef IOS

SenderMobileSynth::SenderMobileSynth(RC1 * rc1)
{
    sy=new mobilesynthview::Widget();    
}

void SenderMobileSynth::cc(int c, int voiceId, int cc, double v1)
{

}

void SenderMobileSynth::pc(int c, int v1)
{
    sy->pc(v1);
}

void SenderMobileSynth::note(int c, int voiceId, double f, int vel)
{
    if(vel>0) {
        sy->noteOn(voiceId, (float)f);
    } else {
        sy->noteOff(voiceId);
    }
}

void SenderMobileSynth::pitch(int c, int voiceId, double f) {
    sy->noteOn(voiceId, (float)f);
}

void SenderMobileSynth::setDestination(QHostAddress a, int p)
{

}

#else


SenderMobileSynth::SenderMobileSynth(RC1 * rc1)
{
    sy= new mobileSynthQT52();
}

void SenderMobileSynth::cc(int c, int voiceId, int cc, double v1)
{

}

void SenderMobileSynth::pc(int c, int v1)
{
}

void SenderMobileSynth::note(int c, int voiceId, double f, int vel)
{
    if(vel>0) {
        sy->noteOn(voiceId, (float)f);
    } else {
        sy->noteOff(voiceId);
    }
}

void SenderMobileSynth::pitch(int c, int voiceId, double f) {
    sy->noteOn(voiceId, (float)f);
}

void SenderMobileSynth::setDestination(QHostAddress a, int p)
{

}

#endif
