#include "platform.h"
#include "sendermobilesynth.h"

#ifdef RC1_IOS

SenderMobileSynth::SenderMobileSynth(RC1 * rc1)
{
    
    syco=new mobilesynthview::Widget();
    sy=new synth::Controller();
    syco->setController(sy);
}

void SenderMobileSynth::cc(int chan, int voiceId, int cc, float v1)
{
    if(cc==2) {
        sy->set_modulation(voiceId,v1);
    } else if(cc==128) {
        sy->set_osc1_wave_type_int(v1);
    } else if(cc==129) {
        sy->setAttack(0, v1*700.0f);
    } else if(cc==130) {
        sy->setDecay(0, v1*700.0f);
    } else if(cc==131) {
        sy->setSustain(0, v1/127.0f);
    } else if(cc==132) {
        sy->setRelease(0, v1*700.0f);
    } else if(cc==133) {
        sy->set_filter_cutoff(v1/127.0f);
    } else if(cc==134) {
        sy->set_filter_resonance(v1/32.0f);
    } else if(cc==135) {
        sy->set_mod_filter_cutoff(v1/127.0f);
    } else if(cc==136) {
        sy->set_mod_filter_resonance(v1/127.0f);
    } else if(cc==137) {
        sy->set_volume(v1/127.0f);
    }
}

void SenderMobileSynth::pc(int chan, int v1) {}

void SenderMobileSynth::noteOn(int chan, int voiceId, float f, int midinote, int pitch, int vel)
{
    sy->NoteOn(voiceId, (float)f);
}

void SenderMobileSynth::noteOff(int chan,int voiceId)
{
    sy->NoteOff(voiceId);
}

void SenderMobileSynth::pitch(int chan, int voiceId, float f, int midinote, int pitch) {
    sy->NoteOn(voiceId, (float)f);
}

void SenderMobileSynth::setDestination(QHostAddress a, int p)
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
    } else if(cc==128) {
        sy->getSyctl()->set_osc1_wave_type_int(v1);
    } else if(cc==129) {
        sy->getSyctl()->setAttack(0, v1*700.0f);
    } else if(cc==130) {
        sy->getSyctl()->setDecay(0, v1*700.0f);
    } else if(cc==131) {
        sy->getSyctl()->setSustain(0, v1/127.0f);
    } else if(cc==132) {
        sy->getSyctl()->setRelease(0, v1*700.0f);
    } else if(cc==133) {
        sy->getSyctl()->set_filter_cutoff(v1/127.0f);
    } else if(cc==134) {
        sy->getSyctl()->set_filter_resonance(v1/32.0f);
    } else if(cc==135) {
        sy->getSyctl()->set_mod_filter_cutoff(v1/127.0f);
    } else if(cc==136) {
        sy->getSyctl()->set_mod_filter_resonance(v1/127.0f);
    } else if(cc==137) {
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
