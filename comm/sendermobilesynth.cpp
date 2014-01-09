#include "sendermobilesynth.h"

#ifndef Q_OS_IOSx

SenderMobileSynth::SenderMobileSynth(RC1 * rc1)
{
    
    syco=new mobilesynthview::Widget();
    sy=new synth::Controller();
    syco->setController(sy);
}

void SenderMobileSynth::cc(int c, int voiceId, int cc, double v1)
{
    if(cc==10) {
        sy->set_osc_pw(voiceId,v1);
    }

}

void SenderMobileSynth::pc(int c, int v1)
{
//    sy->pc(v1);
    sy->set_osc1_wave_type_int(v1%4);

    switch (v1%4) {
        case 0:
            sy->setADSR(0, 10, 300, 0.5, 500);
            break;
            
        case 1:
            sy->setADSR(0, 10, 300, 0.5, 5000);
            break;
            
        case 2:
            sy->setADSR(0, 10, 300, 0.5, 40000);
            break;
            
        case 3:
            sy->setADSR(0, 10, 300, 0.5, 80000);
            break;
            
        default:
            break;
    }
    
}

void SenderMobileSynth::note(int c, int voiceId, double f, int vel)
{
    if(vel>0) {
        sy->NoteOn(voiceId, (float)f);
    } else {
        sy->NoteOff(voiceId);
    }
}

void SenderMobileSynth::pitch(int c, int voiceId, double f) {
    sy->NoteOn(voiceId, (float)f);
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
