#include "platform.h"
#include "sendermobilesynth.h"

#ifdef RC1_IOS

SenderMobileSynth::SenderMobileSynth(RC1 * rc1)
{
    
    syco=new mobilesynthview::Widget();
    sy=new synth::Controller();
    syco->setController(sy);
}

void SenderMobileSynth::cc(int voiceId, int cc, double v1)
{
//    qDebug() << "SenderMobileSynth::cc " << voiceId << " cc " << cc << " v1 " << v1;

    if(cc==2) {
//        qDebug() << "SenderMobileSynth::cc " << voiceId << " cc " << cc << " v1 " << v1;
        sy->set_filter_cutoff(voiceId,v1);
    } else if(cc==1) {
        sy->set_filter_resonance(voiceId,v1);
    } else if(cc==3) {
        sy->set_modulation_amount(voiceId, v1);
    } else if(cc==4) {
        sy->set_modulation_mod_f(voiceId, v1);
    } else if(cc==200) {
        sy->set_osc1_wave_type_int(v1);
    } else if(cc==201) {
        sy->set_lfo_wave_type_int(v1);
    } else if(cc==202) {
        sy->set_modulation_destination(v1);
    } else if(cc==203) {
        switch ((int)v1%4) {
            case 0:
                sy->setADSR(0, 10, 300, 0.5, 500);
                //            sy->set_modulation_destination(synth::Controller::LFO_DEST_NONE);
                break;
                
            case 1:
                sy->setADSR(0, 10, 5000, 0, 5000);
                //            sy->set_modulation_destination(synth::Controller::LFO_DEST_AMP);
                break;
                
            case 2:
                sy->setADSR(0, 10, 300, 0.5, 40000);
                //            sy->set_modulation_destination(synth::Controller::LFO_DEST_PW);
                break;
                
            case 3:
                sy->setADSR(0, 40000, 10, 1, 80000);
                //            sy->set_modulation_destination(synth::Controller::LFO_DEST_PITCH);
                break;
                
            default:
                break;
        }
    } else if(cc==204) {
//        qDebug() << "SenderMobileSynth::cc " << voiceId << " cc " << cc << " v1 " << v1;
        sy->set_filter_resonance(v1);
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

void SenderMobileSynth::pc(int v1)
{
//    sy->pc(v1);
    sy->set_osc1_wave_type_int(v1%5);
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

void SenderMobileSynth::noteOn(int voiceId, double f, int midinote, int pitch, int vel)
{
    sy->NoteOn(voiceId, (float)f);
}

void SenderMobileSynth::noteOff(int voiceId)
{
    sy->NoteOff(voiceId);
}

void SenderMobileSynth::pitch(int voiceId, double f, int midinote, int pitch) {
    sy->NoteOn(voiceId, (float)f);
}

void SenderMobileSynth::setDestination(QHostAddress a, int p)
{

}

#else

SenderMobileSynth::SenderMobileSynth(RC1 * rc1)
{
    sy= new mobileSynthQT52();
    qDebug() << "mobileSynthQt52 created";
}

void SenderMobileSynth::cc(int voiceId, int cc, double v1)
{
    if(cc==2) {
//        qDebug() << "SenderMobileSynth::cc " << voiceId << " cc " << cc << " v1 " << v1;
        sy->getSyctl()->set_filter_cutoff(voiceId,v1);
    } else if(cc==1) {
        sy->getSyctl()->set_filter_resonance(voiceId,v1);
    } else if(cc==3) {
        sy->getSyctl()->set_modulation_amount(voiceId, v1);
    } else if(cc==4) {
        sy->getSyctl()->set_modulation_mod_f(voiceId, v1);
    } else if(cc==200) {
        sy->getSyctl()->set_osc1_wave_type_int(v1);
    } else if(cc==201) {
        sy->getSyctl()->set_lfo_wave_type_int(v1);
    } else if(cc==202) {
        sy->getSyctl()->set_modulation_destination(v1);
    } else if(cc==203) {
        switch ((int)v1%4) {
            case 0:
                sy->getSyctl()->setADSR(0, 10, 300, 0.5, 500);
                break;
            case 1:
                sy->getSyctl()->setADSR(0, 10, 5000, 0, 5000);
                break;
            case 2:
                sy->getSyctl()->setADSR(0, 10, 300, 0.5, 40000);
                break;
            case 3:
                sy->getSyctl()->setADSR(0, 40000, 10, 1, 80000);
                break;

            default:
                break;
        }
    } else if(cc==204) {
        sy->getSyctl()->set_filter_resonance(v1);
    }
}

void SenderMobileSynth::pc(int v1)
{
}

void SenderMobileSynth::noteOn(int voiceId, double f, int midinote, int pitch, int vel)
{
    sy->noteOn(voiceId, (float)f);
}

void SenderMobileSynth::noteOff(int voiceId)
{
    sy->noteOff(voiceId);
}

void SenderMobileSynth::pitch(int voiceId, double f, int midinote, int pitch) {
    sy->noteOn(voiceId, (float)f);
}

void SenderMobileSynth::setDestination(QHostAddress a, int p)
{

}

#endif
