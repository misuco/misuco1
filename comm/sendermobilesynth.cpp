#include "platform.h"
#include "sendermobilesynth.h"

#ifdef RC1_IOS

SenderMobileSynth::SenderMobileSynth(RC1 * rc1)
{
    
    syco=new mobilesynthview::Widget();
    sy=new synth::Controller();
    syco->setController(sy);
}

void SenderMobileSynth::cc(int c, int voiceId, int cc, double v1)
{
    if(cc==12) {
        sy->set_filter_cutoff(voiceId,v1);
    } else if(cc==11) {
        sy->set_filter_resonance(voiceId,v1);
    }
    if(cc==200) {
        sy->getSyctl()->set_osc1_wave_type_int(v1);
    }
    /*
    if(cc==10) {
        sy->set_osc_pw(voiceId,v1);
    }
    if(cc==11) {
        sy->set_modulation_amount(voiceId,v1);
    }
    if(cc==12) {
        sy->set_modulation_frequency(voiceId,1+v1*20);
    }
     */
}

void SenderMobileSynth::pc(int c, int v1)
{
//    sy->pc(v1);
    sy->set_osc1_wave_type_int(v1%2);
    sy->set_modulation_destination(synth::Controller::LFO_DEST_NONE);
    switch (v1%4) {
        case 0:
            sy->setADSR(0, 10, 300, 0.5, 500);
//            sy->set_modulation_destination(synth::Controller::LFO_DEST_NONE);
            break;
            
        case 1:
            sy->setADSR(0, 10, 300, 0.5, 5000);
//            sy->set_modulation_destination(synth::Controller::LFO_DEST_AMP);
            break;
            
        case 2:
            sy->setADSR(0, 10, 300, 0.5, 40000);
//            sy->set_modulation_destination(synth::Controller::LFO_DEST_PW);
            break;
            
        case 3:
            sy->setADSR(0, 10, 300, 0.5, 80000);
//            sy->set_modulation_destination(synth::Controller::LFO_DEST_PITCH);
            break;
            
        default:
            break;
    }
    sy->set_modulation_amount(0);
    /*
    if(v1>7) {
        sy->set_modulation_amount(1.0);
    } else if(v1>3) {
        sy->set_modulation_amount(0.5);
    } else {
        sy->set_modulation_amount(0);
    } */
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
    if(cc==12) {
        sy->getSyctl()->set_filter_cutoff(voiceId,v1);
    } else if(cc==11) {
        sy->getSyctl()->set_filter_resonance(voiceId,v1);
    }
    if(cc==200) {
        sy->getSyctl()->set_osc1_wave_type_int(v1);
    }
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
