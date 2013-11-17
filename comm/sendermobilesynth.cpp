#include "sendermobilesynth.h"



SenderMobileSynth::SenderMobileSynth(RC1 * rc1)
{
    sy=new mobilesynthview::Widget();    
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
        sy->noteOn(f);
        
    } else {
        sy->noteOff(f);
    }
}

void SenderMobileSynth::pitch(int c, int voiceId, double f) {}

void SenderMobileSynth::setDestination(QHostAddress a, int p)
{

}

