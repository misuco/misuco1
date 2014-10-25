#include "platform.h"
#include "sendermobilesynth.h"

#ifdef RC1_IOS

SenderMobileSynth::SenderMobileSynth(RC1 *)
{
    
    syco=new mobilesynthview::Widget();
    sy=new synth::Controller();
    syco->setController(sy);
}

void SenderMobileSynth::cc(int, int voiceId, int cc, float v1)
{
    if(cc==2) {
        sy->set_modulation(voiceId,v1);
    } else if(cc==118) {
        sy->set_osc1_wave_type_int(v1);
    } else if(cc==119) {
        sy->setAttack(0, v1*700.0f);
    } else if(cc==120) {
        sy->setDecay(0, v1*700.0f);
    } else if(cc==121) {
        sy->setSustain(0, v1/127.0f);
    } else if(cc==122) {
        sy->setRelease(0, v1*700.0f);
    } else if(cc==123) {
        sy->set_filter_cutoff(v1/127.0f);
    } else if(cc==124) {
        sy->set_filter_resonance(v1/32.0f);
    } else if(cc==125) {
        sy->set_mod_filter_cutoff(v1/127.0f);
    } else if(cc==126) {
        sy->set_mod_filter_resonance(v1/127.0f);
    } else if(cc==127) {
        sy->set_volume(v1/127.0f);
    }
}

void SenderMobileSynth::pc(int, int) {}

void SenderMobileSynth::noteOn(int, int voiceId, float f, int, int, int)
{
    sy->NoteOn(voiceId, (float)f);
}

void SenderMobileSynth::noteOff(int,int voiceId)
{
    sy->NoteOff(voiceId);
}

void SenderMobileSynth::pitch(int, int voiceId, float f, int, int) {
    sy->NoteOn(voiceId, (float)f);
}

void SenderMobileSynth::setDestination(QHostAddress,int)
{

}

#else

SenderMobileSynth::SenderMobileSynth(RC1 * rc1)
{
    sy= new mobileSynthQT52();
    //qDebug() << "mobileSynthQt52 created";
}

void SenderMobileSynth::cc(int chan, int voiceId, int cc, float v1)
{
    if(cc==2) {
        sy->getSyctl()->set_modulation(voiceId,v1);
    } else if(cc==118) {
        sy->getSyctl()->set_osc1_wave_type_int(v1);
    } else if(cc==119) {
        sy->getSyctl()->setAttack(0, v1*700.0f);
    } else if(cc==120) {
        sy->getSyctl()->setDecay(0, v1*700.0f);
    } else if(cc==121) {
        sy->getSyctl()->setSustain(0, v1/127.0f);
    } else if(cc==122) {
        sy->getSyctl()->setRelease(0, v1*700.0f);
    } else if(cc==123) {
        sy->getSyctl()->set_filter_cutoff(v1/127.0f);
    } else if(cc==124) {
        sy->getSyctl()->set_filter_resonance(v1/32.0f);
    } else if(cc==125) {
        sy->getSyctl()->set_mod_filter_cutoff(v1/127.0f);
    } else if(cc==126) {
        sy->getSyctl()->set_mod_filter_resonance(v1/127.0f);
    } else if(cc==127) {
        sy->getSyctl()->set_volume(v1/127.0f);
    }
}

void SenderMobileSynth::pc(int chan, int v1) {}

void SenderMobileSynth::noteOn(int chan, int voiceId, float f, int midinote, int pitch, int vel)
{
    sy->noteOn(voiceId, (float)f);
}

void SenderMobileSynth::noteOff(int chan, int voiceId)
{
    sy->noteOff(voiceId);
}

void SenderMobileSynth::pitch(int chan, int voiceId, float f, int midinote, int pitch) {
    sy->noteOn(voiceId, (float)f);
}

void SenderMobileSynth::setDestination(QHostAddress a, int p)
{

}

#endif
